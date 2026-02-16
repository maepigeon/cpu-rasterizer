#ifndef RASTERIZER_GEOMETRY
#define RASTERIZER_GEOMETRY
#include <cstdint>

#include <glm/glm.hpp>


struct Line {
    glm::ivec2 v0;
    glm::ivec2 v1;
};
struct Triangle {
    glm::ivec2 v0;
    glm::ivec2 v1;
    glm::ivec2 v2;
};
typedef union {
    struct {
        std::uint8_t r, g, b, a;
    } channels;
    std::uint32_t bits;
} Color;
struct ColoredTriangle {
    Triangle tri;
    Color color;
};

#endif