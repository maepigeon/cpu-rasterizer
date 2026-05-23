#ifndef SUTHERLAND_HODGMAN_CLIPPING_HPP
#define SUTHERLAND_HODGMAN_CLIPPING_HPP

#include <vector>
#include <glm/glm.hpp>

struct ClipVertex {
    glm::vec4 pos;
    glm::vec2 uv;
};

struct ClippedPolygon {
    std::vector<ClipVertex> verts;
    bool valid;
};

enum class ClipPlane {
    Left,
    Right,
    Bottom,
    Top
};

glm::vec2 clipToScreen(const glm::vec4 clip, int width, int height);
bool insidePlane(const glm::vec4& p, ClipPlane plane);
ClipVertex intersectPlane(const ClipVertex& a, const ClipVertex& b, ClipPlane plane);
void clipPolygonAgainstPlane(const std::vector<ClipVertex>& in, std::vector<ClipVertex>& out, ClipPlane plane);
ClippedPolygon clipTriangleFull(const glm::vec4& a, const glm::vec4& b, const glm::vec4& c, 
                                const glm::vec2& uvA, const glm::vec2& uvB, const glm::vec2& uvC);
#endif
