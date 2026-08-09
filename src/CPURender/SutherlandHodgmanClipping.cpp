#include "SutherlandHodgmanClipping.hpp"

bool insidePlane(glm::vec4& p, ClipPlane plane) {
    switch (plane) {
        case ClipPlane::Left:   return p.x >= -p.w;
        case ClipPlane::Right:  return p.x <=  p.w;
        case ClipPlane::Bottom: return p.y >= -p.w;
        case ClipPlane::Top:    return p.y <=  p.w;
        case ClipPlane::Near:   return p.z >= -p.w;
    }
    return false;
}

ClipVertex intersectPlane(ClipVertex& a, ClipVertex& b, ClipPlane plane) {
    float aVal, bVal;
    switch (plane) {
        case ClipPlane::Left:   aVal = a.pos.x + a.pos.w; bVal = b.pos.x + b.pos.w; break;
        case ClipPlane::Right:  aVal = a.pos.w - a.pos.x; bVal = b.pos.w - b.pos.x; break;
        case ClipPlane::Bottom: aVal = a.pos.y + a.pos.w; bVal = b.pos.y + b.pos.w; break;
        case ClipPlane::Top:    aVal = a.pos.w - a.pos.y; bVal = b.pos.w - b.pos.y; break;
        case ClipPlane::Near:   aVal = a.pos.z + a.pos.w; bVal = b.pos.z + b.pos.w; break;
    }
    float denominator = (aVal - bVal);
    if (std::abs(denominator) < 1e-6f) {
        return a; // segment is almost parallel, just pick one
    }
    float t = aVal / denominator;
    t = glm::clamp(t, 0.0f, 1.0f); // This prevents runaway intersections
    // Interpolate both position and UV coordinate
    ClipVertex result;
    result.pos = a.pos + t * (b.pos - a.pos);
    result.uv = a.uv + t * (b.uv - a.uv);
    return result;
}


void clipPolygonAgainstPlane(std::vector<ClipVertex>& in, std::vector<ClipVertex>& out, ClipPlane plane) {
    out.clear();
    if (in.empty()) {
        return;
    }
    for (int i = 0; i < in.size(); ++i) {
        ClipVertex& S = in[i];
        ClipVertex& E = in[(i + 1) % in.size()];
        bool S_in = insidePlane(S.pos, plane);
        bool E_in = insidePlane(E.pos, plane);

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

glm::vec2 clipToScreen(glm::vec4 clip, int width, int height) {
    // avoid divide-by-zero
    if (!std::isfinite(clip.w) || std::abs(clip.w) < 1e-6) {
        return glm::ivec2(-10000, -10000);
    }
    glm::vec3 normalizedDeviceCoordinates = glm::vec3(clip) / clip.w; // [-1, 1]
    if (!std::isfinite(normalizedDeviceCoordinates.x) || !std::isfinite(normalizedDeviceCoordinates.y))
    return glm::ivec2(-10000, -10000);

    float sx = (normalizedDeviceCoordinates.x * 0.5f + 0.5f) * width;
    float sy = (1.0f - (normalizedDeviceCoordinates.y * 0.5f + 0.5f)) * height; // flip Y
    return glm::ivec2((int)sx, (int)sy);
};

ClippedPolygon clipTriangleFull(glm::vec4& a, glm::vec4& b, glm::vec4& c, glm::vec2& uvA, glm::vec2& uvB, glm::vec2& uvC) {
    std::vector<ClipVertex> polyIn;
    std::vector<ClipVertex> polyOut;

    polyIn.push_back({a, uvA});
    polyIn.push_back({b, uvB});
    polyIn.push_back({c, uvC});

    ClipPlane planes[] = {
        ClipPlane::Left,
        ClipPlane::Right,
        ClipPlane::Bottom,
        ClipPlane::Top,
        ClipPlane::Near
    };

    for (int i = 0; i < 4; ++i) {
        clipPolygonAgainstPlane(polyIn, polyOut, planes[i]);
        if (polyOut.empty())
            return { {}, false };
        polyIn.swap(polyOut);
    }

    return { polyIn, true };
}