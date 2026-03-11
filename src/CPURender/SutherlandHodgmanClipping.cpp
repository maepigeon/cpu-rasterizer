#include "SutherlandHodgmanClipping.hpp"

bool insidePlane(const glm::vec4& p, ClipPlane plane) {
    switch (plane) {
        case ClipPlane::Left:   return p.x >= -p.w;
        case ClipPlane::Right:  return p.x <=  p.w;
        case ClipPlane::Bottom: return p.y >= -p.w;
        case ClipPlane::Top:    return p.y <=  p.w;
    }
    return false;
}

glm::vec4 intersectPlane(const glm::vec4& a,
                         const glm::vec4& b,
                         ClipPlane plane)
{
    float aVal, bVal;

    switch (plane) {
        case ClipPlane::Left:   aVal = a.x + a.w; bVal = b.x + b.w; break;
        case ClipPlane::Right:  aVal = a.w - a.x; bVal = b.w - b.x; break;
        case ClipPlane::Bottom: aVal = a.y + a.w; bVal = b.y + b.w; break;
        case ClipPlane::Top:    aVal = a.w - a.y; bVal = b.w - b.y; break;
    }

    float denom = (aVal - bVal);
    if (std::abs(denom) < 1e-6f) {
        return a; // or b; segment is almost parallel, just pick one
    }
    float t = aVal / denom;
    t = glm::clamp(t, 0.0f, 1.0f); // This prevents runaway intersections
    return a + t * (b - a);
}


void clipPolygonAgainstPlane(const std::vector<glm::vec4>& in,
                             std::vector<glm::vec4>& out,
                             ClipPlane plane)
{
    out.clear();
    if (in.empty()) return;

    for (size_t i = 0; i < in.size(); ++i) {
        const glm::vec4& S = in[i];
        const glm::vec4& E = in[(i + 1) % in.size()];

        bool S_in = insidePlane(S, plane);
        bool E_in = insidePlane(E, plane);

        if (S_in && E_in) {
            out.push_back(E);
        } else if (S_in && !E_in) {
            out.push_back(intersectPlane(S, E, plane));
        } else if (!S_in && E_in) {
            out.push_back(intersectPlane(S, E, plane));
            out.push_back(E);
        }
    }
}

glm::vec2 clipToScreen(const glm::vec4 clip, int width, int height) {
    // avoid divide-by-zero
    if (!std::isfinite(clip.w) || std::abs(clip.w) < 1e-6f) {
        return glm::ivec2(-10000, -10000); // clearly off-screen
    }

    glm::vec3 ndc = glm::vec3(clip) / clip.w; // [-1, 1]

    if (!std::isfinite(ndc.x) || !std::isfinite(ndc.y))
    return glm::ivec2(-10000, -10000);

    float sx = (ndc.x * 0.5f + 0.5f) * width;
    float sy = (1.0f - (ndc.y * 0.5f + 0.5f)) * height; // flip Y

    return glm::ivec2((int)sx, (int)sy);
};

ClippedPolygon clipTriangleFull(const glm::vec4& a, const glm::vec4& b,  const glm::vec4& c)
{
    std::vector<glm::vec4> polyIn;
    std::vector<glm::vec4> polyOut;

    polyIn.push_back(a);
    polyIn.push_back(b);
    polyIn.push_back(c);

    ClipPlane planes[] = {
        ClipPlane::Left,
        ClipPlane::Right,
        ClipPlane::Bottom,
        ClipPlane::Top,
    };

    for (int i = 0; i < 4; ++i) {
        clipPolygonAgainstPlane(polyIn, polyOut, planes[i]);
        if (polyOut.empty())
            return { {}, false };
        polyIn.swap(polyOut);
    }

    return { polyIn, true };
}