#include <rasterizer/CPURender/SutherlandHodgmanClipping.hpp>
#include <iostream>
#include <rasterizer/Constants.hpp>


namespace rasterizer {
    bool insidePlane(glm::vec4 p, ClipPlane plane) {
        switch (plane) {
            case ClipPlane::Left: return p.x >= -p.w; break;
            case ClipPlane::Right: return p.x <=  p.w; break;
            case ClipPlane::Bottom: return p.y >= -p.w; break;
            case ClipPlane::Top: return p.y <= p.w; break;
            case ClipPlane::Near: return p.z >= -p.w; break;
            case ClipPlane::Far: return p.z <= p.w; break;
        }
        return false;
    }

    ClipVertex intersectPlane(ClipVertex a, ClipVertex b, ClipPlane plane) {
        float aVal, bVal;
        switch (plane) {
            case ClipPlane::Left:
                aVal = a.pos.x + a.pos.w;
                bVal = b.pos.x + b.pos.w;
                break;
            case ClipPlane::Right:
                aVal = a.pos.w - a.pos.x; 
                bVal = b.pos.w - b.pos.x;
                break;
            case ClipPlane::Bottom: 
                aVal = a.pos.y + a.pos.w;
                bVal = b.pos.y + b.pos.w;
                break;
            case ClipPlane::Top:
                aVal = a.pos.w - a.pos.y;
                bVal = b.pos.w - b.pos.y;
                break;
            case ClipPlane::Near:
                aVal = a.pos.z + a.pos.w;
                bVal = b.pos.z + b.pos.w;
                break;
            case ClipPlane::Far:
                aVal = a.pos.w - a.pos.z;
                bVal = a.pos.w - b.pos.z;
                break;
        }
        float denominator = (aVal - bVal);
        float t = aVal / denominator;

        if (std::abs(denominator) < 1e-8f) {
            t = 0;
        }
        t = glm::clamp(t, 0.0f, 1.0f); // This prevents runaway intersections
        // Interpolate both position and UV coordinate
        ClipVertex result;
        result.pos = a.pos + t * (b.pos - a.pos);
        result.vertexData = lerp(a.vertexData, b.vertexData, t, b.vertexData.MAX_SIZE);
        return result;
    }


    std::vector<ClipVertex> clipPolygonAgainstPlane(std::vector<ClipVertex> in, ClipPlane plane) {
        std::vector<ClipVertex> out;
        if (in.empty()) {
            std::cout << "Attempted to clip a polygon against a plane, but the polygon was empty!";
            return {};
        }
        
        for (int i = 0; i < in.size(); ++i) {
            ClipVertex S = in[i];
            ClipVertex E = in[(i + 1) % in.size()];
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
        return out;
    }

    // Turns a clip-space position to a screen space position
    glm::vec2 clipToScreen(glm::vec4 clip, int width, int height) {
        // avoid divide-by-zero
        if (!std::isfinite(clip.w) || std::abs(clip.w) < epsilon_8) {
            std::cout << "Warning: in clipToScreen, a clip-space position is either extremely close to the camera or extremely far" << std::endl;
            return glm::vec2(-10000.f, -10000.f);
        }
        glm::vec3 ndc = glm::vec3(clip) / clip.w; // [-1, 1]
        if (!std::isfinite(ndc.x) || !std::isfinite(ndc.y)) {
            return glm::vec2(-10000.f, -10000.f);

        }
        float sx = (ndc.x * 0.5f + 0.5f) * width;
        float sy = (1.0f - (ndc.y * 0.5f + 0.5f)) * height; // flip Y
        return glm::vec2(sx, sy);
    };

    // The Sutherland-Hodgman  clipping algorithm takes a polygon and clips it against one boundary plane at a time. 
    // The result is a polygon, which might we might want to then subdivide
    // The vertices a, b, and c all have a per-vertex payload that is recalculated using barycentric interpolation 
    // for new vertices if a vertex is clipped. The vertices positions themselves are in x,y,z,w homogeneous coordinates.
    // At this point in the pipeline, the vertices have been transformed by the transformation matrix and world and view matrixes, 
    // x and y are a non-normalized screen-space position
    // 
    // and w is the distance of the vertex from the position. -z_view. It is used for perspective scaling and clipping against the view frustrum
    // Normalized device coordinates are later constructed from the clip-space coordinated divided by this (the perspective divide)
    // NDC = (x_c / w_c, y_c / w_c, z_c / w_c)
    // Note: at this point, perspective transformation has already been applied
    // Clipping happens in a normalized box such that -w <= x AND y <= w and 0 <= z <= w. This is the last "space" before conversion to ndc (which )
    ClippedPolygon clipTriangleFull(glm::vec4 a, glm::vec4 b, glm::vec4 c, VertexData xA, VertexData xB, VertexData xC) {
        std::vector<ClipVertex> polyIn;
        polyIn.push_back({a, xA});
        polyIn.push_back({b, xB});
        polyIn.push_back({c, xC});
        std::vector<ClipPlane> planes = {
            ClipPlane::Left,
            ClipPlane::Right,
            ClipPlane::Bottom,
            ClipPlane::Top,
            ClipPlane::Near,
            ClipPlane::Far
        };

        // Clip the geometry against each plane
        for (int i = 0; i < planes.size(); i++) {
            std::vector<ClipVertex> polyOut;
            polyOut = clipPolygonAgainstPlane(polyIn, planes[i]);
            if (polyOut.empty()) {
                return { {}, false };
            }
            polyIn.swap(polyOut);
        }

        return { polyIn, true };
    }
}