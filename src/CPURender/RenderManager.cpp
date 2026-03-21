#include "RenderManager.hpp"
#include <iostream>
#include "VertexProcessor.hpp"
#include <glm/glm.hpp>
#include "SutherlandHodgmanClipping.hpp"

RenderManager::RenderManager(SDL_Renderer* sdlRenderer, SDL_Window* window, Color color, int width, int height) {
    bool success = cpuRenderer.createCanvas(sdlRenderer, window, color, width, height);
    cpuRenderer.initGeometry();
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
void RenderManager::renderModel(Model* model, Camera* camera) {
    float PI = 3.14159265358979;
    std::vector<Mesh> meshes = model->getMeshes();
    //model2stdout(meshes);
    VertexProcessor vp;
    vp.modelMatrix = model->worldTransform;
    vp.viewMatrix = camera->getViewTransform();
    vp.projectionMatrix = camera->perspectiveTransform(glm::radians(90.), 1., 0.1, 10.);

    cpuRenderer.clearRenderQueue();

    glm::vec3 p = meshes[0].vertices[0].position;
    glm::vec4 world = vp.modelMatrix * glm::vec4(p, 1.f);
    glm::vec4 view  = vp.viewMatrix  * world;
    glm::vec4 clip  = vp.projectionMatrix * view;
    glm::vec3 ndc   = glm::vec3(clip) / clip.w;
    /*
    std::cout << "world: " << world.x << " " << world.y << " " << world.z << "\n";
    std::cout << "view:  " << view.x  << " " << view.y  << " " << view.z  << "\n";
    std::cout << "clip:  " << clip.x  << " " << clip.y  << " " << clip.z  << " w=" << clip.w << "\n";
    std::cout << "ndc:   " << ndc.x   << " " << ndc.y   << " " << ndc.z   << "\n";
    */
    

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
            
            // Now clip triangle (v0, v1, v2)
            // Backface culling
            
            glm::vec3 a = glm::vec3(v0.viewSpacePos);
            glm::vec3 b = glm::vec3(v1.viewSpacePos);
            glm::vec3 c = glm::vec3(v2.viewSpacePos);            
            
            glm::vec3 ab = b - a;
            glm::vec3 ac = c - a;
            float facing = (ab.x * ac.y - ab.y * ac.x); // 2D cross in view space
            if (facing == 0.) {
                continue; // backface
            }
 
            // Run full Sutherland–Hodgman clipping
            glm::vec4 p0 = v0.clipSpacePos;
            glm::vec4 p1 = v1.clipSpacePos;
            glm::vec4 p2 = v2.clipSpacePos;
            ClippedPolygon clipped = clipTriangleFull(p0, p1, p2);
            //std::cout << "clipped verts: " << clipped.verts.size() << "\n";
            
            // If triangle is fully clipped away, skip it
            if (!clipped.valid || clipped.verts.size() < 3)
                continue;

 
            for (size_t k = 1; k + 1 < clipped.verts.size(); ++k) {
                glm::vec4 t0 = clipped.verts[0];
                glm::vec4 t1 = clipped.verts[k];
                glm::vec4 t2 = clipped.verts[k + 1];

                glm::vec2 s0 = clipToScreen(t0, cpuRenderer.width, cpuRenderer.height);
                glm::vec2 s1 = clipToScreen(t1, cpuRenderer.width, cpuRenderer.height);
                glm::vec2 s2 = clipToScreen(t2, cpuRenderer.width, cpuRenderer.height);

                float abx = s1.x - s0.x;
                float aby = s1.y - s0.y;
                float acx = s2.x - s0.x;
                float acy = s2.y - s0.y;
                float area2 = abx * acy - aby * acx;
                if (std::abs(area2) < 1.0f)
                    continue;

                // Rasterize triangle
                Triangle tri = { s0, s1, s2 };
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