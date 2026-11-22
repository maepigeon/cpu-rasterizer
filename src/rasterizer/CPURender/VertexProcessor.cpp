#include <rasterizer/CPURender/VertexProcessor.hpp>
#include <rasterizer/Mesh.hpp>
#include <iostream>
namespace rasterizer {
    VertexProcessor::VertexProcessor() {};
    VertexProcessor::VertexShaderOutput VertexProcessor::vertexShader(Mesh::Vertex& vertex) {
        glm::vec3 vPos = vertex.position;
        VertexShaderOutput vpo{};
        glm::vec4 worldSpacePos = (modelMatrix * glm::vec4(vPos, 1.0f)); 
        glm::vec4 viewSpacePos = viewMatrix * worldSpacePos;
        vpo.clipSpacePos = projectionMatrix * viewSpacePos;

        vpo.vertexData.data[VertexData::NORMAL] = vertex.normal.x;
        vpo.vertexData.data[VertexData::NORMAL + 1] = vertex.normal.y;
        vpo.vertexData.data[VertexData::NORMAL + 2] = vertex.normal.z;
        vpo.vertexData.data[VertexData::UV] = vertex.texcoord.x;
        vpo.vertexData.data[VertexData::UV + 1] = vertex.texcoord.y;
        for (int i = 0; i < extraFloatCount; i++) {
            vpo.vertexData.data[VertexData::EXTRA + i] = vertex.extraDataFloats[i];
        }
        return vpo;
    }
}