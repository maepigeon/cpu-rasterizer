#include "VertexProcessor.hpp"
#include "Mesh.hpp"
#include <iostream>

VertexProcessor::VertexProcessor() {};
VertexProcessor::VertexShaderOutput VertexProcessor::vertexShader(Mesh::Vertex& vertex) {
    glm::vec3 vPos = vertex.position;
    VertexShaderOutput vpo{};
    glm::vec4 worldSpacePos = (modelMatrix * glm::vec4(vPos, 1.0f)); 
    glm::vec4 viewSpacePos = viewMatrix * worldSpacePos;
    vpo.clipSpacePos = projectionMatrix * viewSpacePos;

    vpo.variablePayloadData.data[VariablePayload::NORMAL] = vertex.normal.x;
    vpo.variablePayloadData.data[VariablePayload::NORMAL + 1] = vertex.normal.y;
    vpo.variablePayloadData.data[VariablePayload::NORMAL + 2] = vertex.normal.z;
    vpo.variablePayloadData.data[VariablePayload::UV] = vertex.texcoord.x;
    vpo.variablePayloadData.data[VariablePayload::UV + 1] = vertex.texcoord.y;
    for (int i = 0; i < extraFloatCount; i++) {
        vpo.variablePayloadData.data[VariablePayload::EXTRA + i] = vertex.extraDataFloats[i];
    }
    return vpo;
}