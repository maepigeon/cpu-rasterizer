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
        std::vector<float> extraDataFloats;
    };
    int extraFloatCount = 0;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    int vertexCount();
    int indexCount();
    void calculateFlatNormals();
	void calculateSmoothNormalsFromWelded();
	void buildWeldMap(float tolerance);
    float calculateTolerance();
    std::vector<uint32_t> unweldedIndexToWeldedIndex_map;
	std::vector<glm::vec3> weldedPositions; // welded vertices data
    void initializeFromWelded(std::vector<glm::vec3>& positions, std::vector<uint32_t>& weldedIndices);
};

#endif