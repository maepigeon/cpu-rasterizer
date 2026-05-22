#ifndef MESH
#define MESH
#include <glm/glm.hpp>
#include <vector> 

class Mesh {
public:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;
    };
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    int vertexCount();
    int indexCount();
};

#endif