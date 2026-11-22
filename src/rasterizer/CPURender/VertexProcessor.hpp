#ifndef CPU_RASTERIZER_VERTEX_PROCESSOR_HPP
#define CPU_RASTERIZER_VERTEX_PROCESSOR_HPP
#include <glm/glm.hpp>
#include <rasterizer/CPURender/RasterizerGeometry.hpp>
#include <rasterizer/Mesh.hpp>

namespace rasterizer {

class VertexProcessor {
    public:
        VertexProcessor();
        glm::mat4x4 modelMatrix;
        glm::mat4x4 viewMatrix;
        glm::mat4x4 projectionMatrix;
        int extraFloatCount = 0;
        struct VertexShaderOutput {
            glm::vec4 clipSpacePos;  //(x,y,z,w)
            VertexData vertexData;
        };
        static inline float getPayload(VertexShaderOutput vso, int index) {
            return getPayload(vso.vertexData, index);
        }
        static inline float getPayload(VertexData vp, int index) {
            return vp.data[VertexData::EXTRA + index];
        }
        static inline glm::vec2 getUV(VertexShaderOutput vso) {
            return getUV(vso.vertexData);
        }
        static inline glm::vec2 getUV(VertexData vp) {
            return {vp.data[VertexData::UV], vp.data[VertexData::UV + 1]};
        }
        static inline glm::vec3 getNormal(VertexShaderOutput vso) {
            return getNormal(vso.vertexData);
        }
        static inline glm::vec3 getNormal(VertexData vp) {
            return {vp.data[VertexData::NORMAL], vp.data[VertexData::NORMAL + 1], vp.data[VertexData::NORMAL + 2]};
        }
        VertexProcessor::VertexShaderOutput vertexShader(Mesh::Vertex& vertex);
    };
}
#endif