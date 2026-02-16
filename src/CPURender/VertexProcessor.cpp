#include "VertexProcessor.hpp"
#include "Mesh.hpp"
#include <iostream>

VertexProcessor::VertexProcessor() {};
VertexProcessor::VertexShaderOutput VertexProcessor::vertexShader(Mesh::Vertex vertex) {
    glm::vec3 vPos = vertex.position;
    struct VertexShaderOutput vpo;
    vpo.worldSpacePos = (modelMatrix * glm::vec4(vPos, 1.));
    //vpo.clipSpacePos = vPos;
    glm::vec4 clip = projectionMatrix * viewMatrix * modelMatrix * glm::vec4(vPos, 1.);
    vpo.clipSpacePos = clip;

    glm::vec4 viewPos = viewMatrix * modelMatrix * glm::vec4(vPos, 1.0f);
    std::cout << "view z = " << viewPos.z << "\n";

    //vpo.normal = vertex.normal;
    //vpo.texcoord = vertex.texcoord;
    return vpo;
}