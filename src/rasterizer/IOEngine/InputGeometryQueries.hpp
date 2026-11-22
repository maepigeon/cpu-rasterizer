#ifndef CPU_RASTERIZER_GEOMETRYQUERIES_HPP
#define CPU_RASTERIZER_GEOMETRYQUERIES_HPP

#include <SDL3/SDL_events.h>
#include <rasterizer/CPURender/Rasterizer.hpp>

namespace rasterizer {
    struct PickResult {
        bool hit = false;
        int meshIndex = -1;
        int triangleIndex = -1;
        float alpha, beta, gamma; // Barycentric coordinates of the hit point in the triangle
    };

    class InputGeometryQueries {
    public:
        InputGeometryQueries(Rasterizer* rasterizer, float scale);
        PickResult pickAtWindowPos(float wx, float wy);
    private:
        Rasterizer* rasterizer;
        float scale;
    };
}
#endif