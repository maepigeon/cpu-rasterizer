#include <rasterizer/CPURender/RenderManager.hpp>
#include <rasterizer/CPURender/RasterizerGeometry.hpp>
#include <rasterizer/Mesh.hpp>
#include <rasterizer/CPURender/SutherlandHodgmanClipping.hpp>
#include <rasterizer/Constants.hpp>
#include <rasterizer/CPURender/VertexProcessor.hpp>
#include <glm/glm.hpp>
#include <iostream>


namespace rasterizer {
    RenderManager::RenderManager(SDL_Renderer* sdlRenderer, SDL_Window* window, Color color, int width, int height) {
        bool success = cpuRasterizer.createCanvas(sdlRenderer, window, color, width, height);
    }
    void RenderManager::destroy() {
        cpuRasterizer.destroy();
    }
    float triangleArea2(glm::vec2 a, glm::vec2 b, glm::vec2 c) {
        float abx = b.x - a.x;
        float aby = b.y - a.y;
        float acx = c.x - a.x;
        float acy = c.y - a.y;
        return abx * acy - aby * acx;
    }

    // Interpolate UV coordinates at point p inside triangle v0,v1,v2 with vertex UVs uv0, uv1, uv2
    glm::vec2 interpUV(glm::vec2 p, 
                    glm::vec2 v0Screen, glm::vec2 v1Screen, glm::vec2 v2Screen,
                    glm::vec2 uv0, glm::vec2 uv1, glm::vec2 uv2) {
        // formula: uv = (A_xbc * uv0 + A_xca * uv1 + A_xab * uv2) / A_abc
        // This gets the barycentric coordinates of p with respect to the triangle, then uses those to interpolate the UVs.
        // denominator of barycentric coordinates formula, also equal to 2x area of the triangle
        float denominator = (v0Screen.y - v2Screen.y) * (v0Screen.x-v2Screen.x) 
            + (v2Screen.x - v1Screen.x) * (v0Screen.y - v2Screen.y);
        if (std::abs(denominator) < epsilon_8) {
            return uv0; // Degenerate triangle, just return uv0
        }
        // Calculate weights of the 3 screen-space p
        float a = (
            (v1Screen.y - v2Screen.y) * (p.x-v2Screen.x) + 
            (v2Screen.x-v1Screen.x)*(p.y-v2Screen.y)
        ) / denominator;
        float b = (
            (v2Screen.y - v0Screen.y) * (p.x-v2Screen.x) + 
            (v0Screen.x-v2Screen.x) * (p.y-v2Screen.y)
        ) / denominator;
        float c = 1.0f - a - b;
        return a * uv0 + b * uv1 + c * uv2;
    }
    void RenderManager::renderModel(ModelIndex modelIdx, ModelsManager& modelsManager, Camera* camera, double aspect) {
        Model* model = &modelsManager.getModel(modelIdx);
        float PI = 3.14159265358979f;
        std::vector<Mesh> meshes = model->getMeshes();
        //model2stdout(meshes);
        VertexProcessor vp;
        vp.modelMatrix = model->worldTransform;
        vp.viewMatrix = camera->getViewTransform();
        vp.projectionMatrix = camera->perspectiveTransform(glm::radians(90.), aspect, 0.1, 10.);

        // Overvew of the vertices transformed through stages of the pipeline:
        // vec3 p = meshes[0].vertices[0].position;
        // vec4 world = vp.modelMatrix * glm::vec4(p, 1.f);
        // vec4 view = vp.viewMatrix  * world;
        // vec4 clip = vp.projectionMatrix * view;
        // vec3 ndc = glm::vec3(clip) / clip.w;

        cpuRasterizer.clearRenderQueue();

        if (model->nTextures() > 1) {
            //std::cout << "Rasterizer supports meshes with only one u,v texture image at the moment!\n";
            cpuRasterizer.setTexture(model->getTexture(0));
        } else if (model->nTextures() == 1) {
            //std::cout << "Mesh has one texture image!\n";
            cpuRasterizer.setTexture(model->getTexture(0));
        } 
        // Vertex shader
        int meshIndex = 0;
        for (Mesh& mesh : meshes) {
            mesh.buildWeldMap(mesh.calculateTolerance());
            mesh.calculateSmoothNormalsFromWelded();

            // Calculate vertex normals
            int vertexCount = mesh.vertexCount();
            VertexProcessor::VertexShaderOutput vpo[vertexCount];
            vp.modelMatrix = model->worldTransform;
            for (int i = 0; i < vertexCount; i++) {
                vpo[i] = vp.vertexShader(mesh.vertices[i]);
            }
            int triCount = mesh.indexCount() / 3;

            // Clipping stage (make sure the triangle is in the view frustrum)
            for (int t_i = 0; t_i < triCount; t_i++) {
                int i0 = mesh.indices[t_i*3];
                int i1 = mesh.indices[t_i*3+1];
                int i2 = mesh.indices[t_i*3+2];

                VertexProcessor::VertexShaderOutput v0 = vpo[i0];
                VertexProcessor::VertexShaderOutput v1 = vpo[i1];
                VertexProcessor::VertexShaderOutput v2 = vpo[i2];

                // Run Sutherland–Hodgman clipping with UV coordinates
                glm::vec4 p0 = v0.clipSpacePos;
                glm::vec4 p1 = v1.clipSpacePos;
                glm::vec4 p2 = v2.clipSpacePos;
                ClippedPolygon clipped = clipTriangleFull(p0, p1, p2, v0.vertexData, v1.vertexData, v2.vertexData);
                
                // If triangle is fully clipped away, skip it
                if (!clipped.valid || clipped.verts.size() < 3)
                    continue;
    
                // Clipping a triangle may can result in a higher-n-gon. Split these into triangles and insert them into the render queue, ready to be rasterized
                for (int k = 1; k + 1 < clipped.verts.size(); k++) {
                    glm::vec4 c0 = clipped.verts[0].pos; // c0 is the common vertex between all triangles. the polygon is split up like a fan. 
                    glm::vec4 c1 = clipped.verts[k].pos;
                    glm::vec4 c2 = clipped.verts[k + 1].pos;
                    VertexData a0 = clipped.verts[0].vertexData;
                    VertexData a1 = clipped.verts[k].vertexData;
                    VertexData a2 = clipped.verts[k + 1].vertexData;

                    glm::vec2 screen0 = clipToScreen(c0, cpuRasterizer.width, cpuRasterizer.height);
                    glm::vec2 screen1 = clipToScreen(c1, cpuRasterizer.width, cpuRasterizer.height);
                    glm::vec2 screen2 = clipToScreen(c2, cpuRasterizer.width, cpuRasterizer.height);
                    float signedArea = (screen1.x - screen0.x) * (screen2.y - screen0.y) 
                        - (screen1.y - screen0.y) * (screen2.x - screen0.x);
                    if (signedArea >= epsilon_8) {
                        continue; //backface culling. checking that the area is greater than epsilon also prevents degenerate slivers
                    }
                    // Create textured triangle with interpolated UVs from clipping
                    float perspectiveCorrection0 = 1.0f / c0.w;
                    float perspectiveCorrection1 = 1.0f / c1.w;
                    float perspectiveCorrection2 = 1.0f / c2.w;

                    float zNDC0 = (c0.z * perspectiveCorrection0) * 0.5f + 0.5f; // depthV0
                    float zNDC1 = (c1.z * perspectiveCorrection1) * 0.5f + 0.5f; //depthV1
                    float zNDC2 = (c2.z * perspectiveCorrection2) * 0.5f + 0.5f; //depthV2

                    ShadedTriangle tri = { screen0, screen1, screen2, zNDC0, zNDC1, zNDC2, perspectiveCorrection0, perspectiveCorrection1, perspectiveCorrection2, a0, a1, a2 };
                    cpuRasterizer.renderQueueInsert(tri, TriSource{ meshIndex, t_i});
                }
            }
            meshIndex++;
        }
        cpuRasterizer.update();
    }
    void model2stdout(std::vector<Mesh> meshes) {
        for (Mesh mesh : meshes) {
            std::cout << "#indices: " << mesh.indexCount() << ", #vertices: " << mesh.vertexCount() << std::endl;
            std::cout << "Indices: ";
            for (int i = 0; i < mesh.indexCount(); i++) {
                std::cout << mesh.indices[i] << ", ";
            }
            std::cout << std::endl;
            std::cout << "Vertices: ";
            for (int i = 0; i < mesh.vertexCount(); i++) {
                std::cout << mesh.vertices[i].position.x << "," << mesh.vertices[i].position.y << "," << mesh.vertices[i].position.z << " | ";
            }
            std::cout << std::endl;
        }
    }
}