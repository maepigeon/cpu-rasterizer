#ifndef SUTHERLAND_HODGMAN_CLIPPING_HPP
#define SUTHERLAND_HODGMAN_CLIPPING_HPP

#include <vector>
#include <glm/glm.hpp>

struct ClippedPolygon {
    std::vector<glm::vec4> verts;
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
glm::vec4 intersectPlane(const glm::vec4& a, const glm::vec4& b, ClipPlane plane);
void clipPolygonAgainstPlane(const std::vector<glm::vec4>& in, std::vector<glm::vec4>& out, ClipPlane plane);
ClippedPolygon clipTriangleFull(const glm::vec4& a, const glm::vec4& b, const glm::vec4& c);
#endif
