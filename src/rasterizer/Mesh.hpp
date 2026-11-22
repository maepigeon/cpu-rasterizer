#ifndef CPU_RASTERIZER_MESH_HPP
#define CPU_RASTERIZER_MESH_HPP
#include <glm/glm.hpp>
#include <vector> 
namespace rasterizer {
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
        

        // |  NORMAL CALCULATIONS RELATED!!  |
        // V                                 V
         enum WELDED_STATE {
            WELDED_DATA_NOT_CALCULATED,
            IS_WELDED,
            NOT_WELDED
        };
        WELDED_STATE isWelded = WELDED_DATA_NOT_CALCULATED;
        void calculateFlatNormals();
        // A mesh may or may not be given in welded format.
        // If not welded, the mesh has duplicate vertices. We can use this to calculate a "Flat", single normal direction 
        // over the triangle, but in this case the normals when interpolated over a triangle barycentrically do not vary
        // in other words, any position on the triangle is shaded with a uniform normal
        // However, even if the mesh is given in non-welded format, we can weld them by finding vertices that meet together at the same point!
        // If we DO have welded normals, we can get smoother normal interpolation over the triangle. HOW?
        // We get the flat normals of all of the triangles in a 1-ring from the vertex, sum their NON-normalized Normal vectors, and
        // voila, we have a nice approximation of the average normal at that vertex! When we render the mesh, we want to query a 
        // normal direction from a point on a triangle to calculate the lighting. If we have have a different normal approximation at each vertex,
        // we can do barycentric interpolation of the 3 normal vectors. This gives smooth lighting over the triangle face :3
        void calculateSmoothNormalsFromWelded();
        void buildWeldMap(float tolerance); 
        float calculateTolerance();

        // this array is used to get a weldedVertex from unweldedIndexToWeldedIndex_map[unweldedVertexIndex]
        std::vector<uint32_t> unweldedIndexToWeldedIndex_map; 
        std::vector<glm::vec3> weldedPositions; // welded vertices data
        void replaceWithWelded(std::vector<glm::vec3>& positions, std::vector<uint32_t>& weldedIndices);
        std::vector<glm::ivec3> getWeldedTriangles();
    };
}

#endif