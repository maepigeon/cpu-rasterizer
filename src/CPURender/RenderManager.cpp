#include "RenderManager.hpp"
#include <iostream>
#include "VertexProcessor.hpp"
#include <glm/glm.hpp>

RenderManager::RenderManager(SDL_Renderer* sdlRenderer, SDL_Window* window, Color color, int width, int height) {
    bool success = cpuRenderer.createCanvas(sdlRenderer, window, color, width, height);
    cpuRenderer.initGeometry();
}

void RenderManager::destroy() {
    cpuRenderer.destroy();
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

    auto p = meshes[0].vertices[0].position;
    glm::vec4 world = vp.modelMatrix * glm::vec4(p, 1.f);
    glm::vec4 view  = vp.viewMatrix  * world;
    glm::vec4 clip  = vp.projectionMatrix * view;
    glm::vec3 ndc   = glm::vec3(clip) / clip.w;

    std::cout << "world: " << world.x << " " << world.y << " " << world.z << "\n";
    std::cout << "view:  " << view.x  << " " << view.y  << " " << view.z  << "\n";
    std::cout << "clip:  " << clip.x  << " " << clip.y  << " " << clip.z  << " w=" << clip.w << "\n";
    std::cout << "ndc:   " << ndc.x   << " " << ndc.y   << " " << ndc.z   << "\n";

    

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

            // Rasterize triangle
            // 3. Pixel processing -> Pixel shading / texturing
            // 4. Merging -> Store each pixel's info in a color buffer (visibility determined using depth/z buffer)
            // 5. Render transparent objects from front to back.

            Triangle screenSpaceTriangle = {
                clipToScreen(v0.clipSpacePos),
                clipToScreen(v1.clipSpacePos),
                clipToScreen(v2.clipSpacePos)
            };
            cpuRenderer.renderQueueInsert(screenSpaceTriangle);
            // 6. Stencil buffer records locations of the rendered primitive (8 bits per pixel). Controls rendering into the color buffer and z-buffer
            // 7. Double buffering - swap color buffer each frame
        }
    }
    cpuRenderer.update();
}
glm::vec2 RenderManager::clipToScreen(const glm::vec4 clip) {
    // avoid divide-by-zero
    if (clip.w == 0.0f) {
        return glm::ivec2(0, 0);
        std::cout << "WARNING| CLIP.w == 0!!!" << std::endl;
    }

    glm::vec3 ndc = glm::vec3(clip) / clip.w; // [-1, 1]
    std::cout << "NDC: " << ndc.x << " " << ndc.y << " " << ndc.z << "\n";

    float sx = (ndc.x * 0.5f + 0.5f) * cpuRenderer.width;
    float sy = (1.0f - (ndc.y * 0.5f + 0.5f)) * cpuRenderer.height; // flip Y

    return glm::ivec2((int)sx, (int)sy);
};



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