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
        
        mesh.buildWeldMap(mesh.calculateTolerance());
        mesh.calculateSmoothNormalsFromWelded();
        std::cout << "Calculating vertex normals" << std::endl;

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

            // Run Sutherland–Hodgman clipping with UV coordinates
            glm::vec4 p0 = v0.clipSpacePos;
            glm::vec4 p1 = v1.clipSpacePos;
            glm::vec4 p2 = v2.clipSpacePos;
            ClippedPolygon clipped = clipTriangleFull(p0, p1, p2, v0.variablePayloadData, v1.variablePayloadData, v2.variablePayloadData);
            
            // If triangle is fully clipped away, skip it
            if (!clipped.valid || clipped.verts.size() < 3)
                continue;
 
            for (int k = 1; k + 1 < clipped.verts.size(); k++) {
                glm::vec4 c0 = clipped.verts[0].pos;
                glm::vec4 c1 = clipped.verts[k].pos;
                glm::vec4 c2 = clipped.verts[k + 1].pos;
                VariablePayload a0 = clipped.verts[0].payload;
                VariablePayload a1 = clipped.verts[k].payload;
                VariablePayload a2 = clipped.verts[k + 1].payload;

                glm::vec2 screen0 = clipToScreen(c0, cpuRenderer.width, cpuRenderer.height);
                glm::vec2 screen1 = clipToScreen(c1, cpuRenderer.width, cpuRenderer.height);
                glm::vec2 screen2 = clipToScreen(c2, cpuRenderer.width, cpuRenderer.height);
                float signedArea = (screen1.x - screen0.x) * (screen2.y - screen0.y) - (screen1.y - screen0.y) * (screen2.x - screen0.x);
                if (signedArea <= 0.f) {
                    continue; //backface culling
                }
                // Create textured triangle with interpolated UVs from clipping
                float depthV0 = (c0.z / c0.w) * 0.5f + 0.5f;
                float depthV1 = (c1.z / c1.w) * 0.5f + 0.5f;
                float depthV2 = (c2.z / c2.w) * 0.5f + 0.5f;

                float perspectiveCorrection0 = 1.0f / c0.w;
                float perspectiveCorrection1 = 1.0f / c1.w;
                float perspectiveCorrection2 = 1.0f / c2.w;

                float zNDC0 = (c0.z * perspectiveCorrection0) * 0.5f + 0.5f;
                float zNDC1 = (c1.z * perspectiveCorrection1) * 0.5f + 0.5f;
                float zNDC2 = (c2.z * perspectiveCorrection2) * 0.5f + 0.5f;

                ShadedTriangle tri = { screen0, screen1, screen2, zNDC0, zNDC1, zNDC2, perspectiveCorrection0, perspectiveCorrection1, perspectiveCorrection2, a0, a1, a2 };
                cpuRenderer.renderQueueInsert(tri);
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