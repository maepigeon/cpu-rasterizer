#ifndef CPU_RASTERIZER_SUTHERLAND_HODGMAN_CLIPPING_HPP
#define CPU_RASTERIZER_SUTHERLAND_HODGMAN_CLIPPING_HPP

#include <vector>
#include <glm/glm.hpp>
#include <rasterizer/CPURender/RasterizerGeometry.hpp>

namespace rasterizer {
    // A clipped triangle can result in a higher-n-gon than a triangle. These data structures stand to collect the resulting geometry.
    // The clipVertices of the clippedPolygon are either original vertices with the original per-vertex data, or they are barycentrically
    // interpolated data of resulting clipped vertices which lie along the boundary of the clip volume
    struct ClipVertex {
        glm::vec4 pos;
        VertexData vertexData;
    };
    struct ClippedPolygon {
        std::vector<ClipVertex> verts;
        bool valid;
    };
    enum class ClipPlane {
        Left, Right, Bottom, Top, Near, Far
    };
    //struct ClipPlane {

    //}

    glm::vec2 clipToScreen(glm::vec4 clip, int width, int height);
    bool insidePlane(glm::vec4 p, ClipPlane plane);
    ClipVertex intersectPlane(ClipVertex a, ClipVertex b, ClipPlane plane);
    std::vector<ClipVertex> clipPolygonAgainstPlane(std::vector<ClipVertex> in, ClipPlane plane);
    ClippedPolygon clipTriangleFull(glm::vec4 a, glm::vec4 b, glm::vec4 c, VertexData xA, VertexData xB, VertexData xC);
}
#endif
