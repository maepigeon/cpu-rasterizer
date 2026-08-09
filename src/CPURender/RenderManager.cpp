#include "RenderManager.hpp"
#include <iostream>
#include "VertexProcessor.hpp"
#include <glm/glm.hpp>
#include "SutherlandHodgmanClipping.hpp"

RenderManager::RenderManager(SDL_Renderer* sdlRenderer, SDL_Window* window, Color color, int width, int height) {
    bool success = cpuRenderer.createCanvas(sdlRenderer, window, color, width, height);
    //cpuRenderer.Test();
}

void RenderManager::destroy() {
    cpuRenderer.destroy();
}

float triangleArea2(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c) {
    float abx = b.x - a.x;
    float aby = b.y - a.y;
    float acx = c.x - a.x;
    float acy = c.y - a.y;
    return abx * acy - aby * acx;
}

// Interpolate UV coordinates at point p inside triangle os0os1os2 with vertex UVs uv0, uv1, uv2
glm::vec2 interpUV(glm::vec2 p, 
                   glm::vec2 os0, glm::vec2 os1, glm::vec2 os2,
                   glm::vec2 uv0, glm::vec2 uv1, glm::vec2 uv2) {
    // formula: uv = (A_xbc * uv0 + A_xca * uv1 + A_xab * uv2) / A_abc
    // This gets the barycentric coordinates of p with respect to triangle os0os1os2, then uses those to interpolate the UVs.
    // denominator of barycentric coordinates formula, also equal to 2x area of triangle os0os1os2
    float denominator = (os1.y-os2.y)*(os0.x-os2.x) + (os2.x-os1.x)*(os0.y-os2.y);
    if (std::abs(denominator) < 1e-6f) return uv0; // Degenerate triangle, just return uv0
    float a = ((os1.y-os2.y)*(p.x-os2.x) + (os2.x-os1.x)*(p.y-os2.y)) / denominator;
    float b = ((os2.y-os0.y)*(p.x-os2.x) + (os0.x-os2.x)*(p.y-os2.y)) / denominator;
    float c = 1.0f - a - b;
    return a * uv0 + b * uv1 + c * uv2;
}
void RenderManager::renderModel(Model* model, Camera* camera, double aspect, SDL_Surface* texture) {
    float PI = 3.14159265358979f;
    std::vector<Mesh> meshes = model->getMeshes();
    //model2stdout(meshes);
    VertexProcessor vp;
    vp.modelMatrix = model->worldTransform;
    vp.viewMatrix = camera->getViewTransform();
    vp.projectionMatrix = camera->perspectiveTransform(glm::radians(90.), aspect, 0.1, 10.);

    cpuRenderer.clearRenderQueue();

    glm::vec3 p = meshes[0].vertices[0].position;
    glm::vec4 world = vp.modelMatrix * glm::vec4(p, 1.f);
    glm::vec4 view  = vp.viewMatrix  * world;
    glm::vec4 clip  = vp.projectionMatrix * view;
    glm::vec3 ndc   = glm::vec3(clip) / clip.w;

    
    cpuRenderer.setTexture(texture);


    // Vertex shader
    for (Mesh& mesh : meshes) {
        int vertexCount = mesh.vertexCount();
        VertexProcessor::VertexShaderOutput vpo[vertexCount];
        vp.modelMatrix = model->worldTransform;
        for (int i = 0; i < vertexCount; i++) {
            vpo[i] = vp.vertexShader(mesh.vertices[i]);
        }
        int triCount = mesh.indexCount() / 3;


        // Clipping stage (make sure the triangle is in the view frustrum)
        for (int i = 0; i < triCount; i++) {
            int i0 = mesh.indices[i*3];
            int i1 = mesh.indices[i*3+1];
            int i2 = mesh.indices[i*3+2];

            VertexProcessor::VertexShaderOutput v0 = vpo[i0];
            VertexProcessor::VertexShaderOutput v1 = vpo[i1];
            VertexProcessor::VertexShaderOutput v2 = vpo[i2];
        
 
            // Run full Sutherland–Hodgman clipping with UV coordinates
            glm::vec4 p0 = v0.clipSpacePos;
            glm::vec4 p1 = v1.clipSpacePos;
            glm::vec4 p2 = v2.clipSpacePos;
            glm::vec2 uv0 = v0.texcoord;
            glm::vec2 uv1 = v1.texcoord;
            glm::vec2 uv2 = v2.texcoord;
            ClippedPolygon clipped = clipTriangleFull(p0, p1, p2, uv0, uv1, uv2);
            //std::cout << "clipped verts: " << clipped.verts.size() << "\n";
            
            // If triangle is fully clipped away, skip it
            if (!clipped.valid || clipped.verts.size() < 3)
                continue;

 
            for (size_t k = 1; k + 1 < clipped.verts.size(); ++k) {
                glm::vec4 clippedV0Pos = clipped.verts[0].pos;
                glm::vec4 clippedV1Pos = clipped.verts[k].pos;
                glm::vec4 clippedV2Pos = clipped.verts[k + 1].pos;

                glm::vec2 screenClippedV0Pos = clipToScreen(clippedV0Pos, cpuRenderer.width, cpuRenderer.height);
                glm::vec2 screenClippedV1Pos = clipToScreen(clippedV1Pos, cpuRenderer.width, cpuRenderer.height);
                glm::vec2 screenClippedV2Pos = clipToScreen(clippedV2Pos, cpuRenderer.width, cpuRenderer.height);
                float signedArea = (screenClippedV1Pos.x - screenClippedV0Pos.x) * (screenClippedV2Pos.y - screenClippedV0Pos.y) - (screenClippedV1Pos.y - screenClippedV0Pos.y) * (screenClippedV2Pos.x - screenClippedV0Pos.x);
                if (signedArea <= 0.f) {
                    continue; //backface culling
                }

                // Get interpolated UV coordinates directly from clipped polygon
                glm::vec2 clipUV0 = clipped.verts[0].uv;
                glm::vec2 clipUV1 = clipped.verts[k].uv;
                glm::vec2 clipUV2 = clipped.verts[k + 1].uv;

                // Create textured triangle with interpolated UVs from clipping
                float depthV0 = (clippedV0Pos.z / clippedV0Pos.w) * 0.5f + 0.5f;
                float depthV1 = (clippedV1Pos.z / clippedV1Pos.w) * 0.5f + 0.5f;
                float depthV2 = (clippedV2Pos.z / clippedV2Pos.w) * 0.5f + 0.5f;
                UVNormalDepthAttribute a0{ clipUV0, glm::vec3(0.0f), depthV0 };
                UVNormalDepthAttribute a1{ clipUV1, glm::vec3(0.0f), depthV1 };
                UVNormalDepthAttribute a2{ clipUV2, glm::vec3(0.0f), depthV2 };
                FullFeaturedTriangle tri = { screenClippedV0Pos, screenClippedV1Pos, screenClippedV2Pos, a0, a1, a2 };
                cpuRenderer.renderQueueInsert(tri);


                //TODO
                    // 1. Pixel processing -> Pixel shading / texturing
                    // 2. Merging -> Store each pixel's info in a color buffer (visibility determined using depth/z buffer)
                    // 3. Render transparent objects from front to back.
                    // 4. Stencil buffer records locations of the rendered primitive (8 bits per pixel). Controls rendering into the color buffer and z-buffer
                    // 5. Double buffering - swap color buffer each frame
            }
        }
    }
    cpuRenderer.update();
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