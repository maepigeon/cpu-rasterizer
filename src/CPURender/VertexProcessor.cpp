#include "VertexProcessor.hpp"
#include "Mesh.hpp"
#include <iostream>

VertexProcessor::VertexProcessor() {};
VertexProcessor::VertexShaderOutput VertexProcessor::vertexShader(const Mesh::Vertex& vertex) {
    glm::vec3 vPos = vertex.position;
    VertexShaderOutput vpo{};
    vpo.worldSpacePos = (modelMatrix * glm::vec4(vPos, 1.0f)); 
    vpo.viewSpacePos = viewMatrix * vpo.worldSpacePos;
    vpo.clipSpacePos = projectionMatrix * vpo.viewSpacePos;
    std::cout << "view z = " << vpo.viewSpacePos.z << "\n";

    //vpo.normal = vertex.normal;
    //vpo.texcoord = vertex.texcoord;
    return vpo;
}