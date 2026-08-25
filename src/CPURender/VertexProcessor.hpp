#ifndef VERTEX_PROCESSOR
#define VERTEX_PROCESSOR
#include <glm/glm.hpp>
#include "Mesh.hpp"
#include "RasterizerGeometry.hpp"
class VertexProcessor {
public:
    VertexProcessor();
    glm::mat4x4 modelMatrix;
    glm::mat4x4 viewMatrix;
    glm::mat4x4 projectionMatrix;
    int extraFloatCount = 0;
    struct VertexShaderOutput {
        glm::vec4 clipSpacePos;  //(x,y,z,w)
        VariablePayload variablePayloadData;
    };
    static inline float getPayload(VertexShaderOutput vso, int index) {
        return getPayload(vso.variablePayloadData, index);
    }
    static inline float getPayload(VariablePayload vp, int index) {
        return vp.data[VariablePayload::EXTRA + index];
    }
    static inline glm::vec2 getUV(VertexShaderOutput vso) {
        return getUV(vso.variablePayloadData);
    }
    static inline glm::vec2 getUV(VariablePayload vp) {
        return {vp.data[VariablePayload::UV], vp.data[VariablePayload::UV + 1]};
    }
    static inline glm::vec3 getNormal(VertexShaderOutput vso) {
        return getNormal(vso.variablePayloadData);
    }
    static inline glm::vec3 getNormal(VariablePayload vp) {
        return {vp.data[VariablePayload::NORMAL], vp.data[VariablePayload::NORMAL + 1], vp.data[VariablePayload::NORMAL + 2]};
    }
    VertexProcessor::VertexShaderOutput vertexShader(Mesh::Vertex& vertex);
};
#endif