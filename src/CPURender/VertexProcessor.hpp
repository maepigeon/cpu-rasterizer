#ifndef VERTEX_PROCESSOR
#define VERTEX_PROCESSOR
#include <glm/glm.hpp>
#include "Mesh.hpp"
class VertexProcessor {
public:
    VertexProcessor();
    glm::mat4x4 modelMatrix;
    glm::mat4x4 viewMatrix;
    glm::mat4x4 projectionMatrix;
    struct VertexShaderOutput {
        glm::vec4 worldSpacePos; //(x,y,z)
        glm::vec4 viewSpacePos;  //(x,y,z,1)
        glm::vec4 clipSpacePos;  //(x,y,z,w)
        //glm::vec3 normal;
        //glm::vec2 texcoord;
    };
    VertexShaderOutput vertexShader(const Mesh::Vertex& vertex);
};
#endif