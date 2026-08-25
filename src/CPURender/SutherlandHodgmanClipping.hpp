#ifndef SUTHERLAND_HODGMAN_CLIPPING_HPP
#define SUTHERLAND_HODGMAN_CLIPPING_HPP

#include <vector>
#include <glm/glm.hpp>
#include "RasterizerGeometry.hpp"

struct ClipVertex {
    glm::vec4 pos;
    VariablePayload payload;
    int extraFloatCount = 0;
};

struct ClippedPolygon {
    std::vector<ClipVertex> verts;
    bool valid;
};

enum class ClipPlane {
    Left, Right, Bottom, Top, Near
};

glm::vec2 clipToScreen(glm::vec4 clip, int width, int height);
bool insidePlane(glm::vec4& p, ClipPlane plane);
ClipVertex intersectPlane(ClipVertex& a, ClipVertex& b, ClipPlane plane);
void clipPolygonAgainstPlane(std::vector<ClipVertex>& in, std::vector<ClipVertex>& out, ClipPlane plane);
ClippedPolygon clipTriangleFull(glm::vec4& a, glm::vec4& b, glm::vec4& c, VariablePayload& xA, VariablePayload& xB, VariablePayload& xC);
#endif
