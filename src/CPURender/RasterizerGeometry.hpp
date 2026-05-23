#ifndef RASTERIZER_GEOMETRY
#define RASTERIZER_GEOMETRY
#include <cstdint>

#include <glm/glm.hpp>
#include <SDL3/SDL.h>

struct Line {
    glm::ivec2 v0;
    glm::ivec2 v1;
};

typedef union {
    struct {
        std::uint8_t r, g, b, a;
    } channels;
    std::uint32_t bits;
} Color;

// Attribute types
struct NoAttribute {};

struct ColorAttribute {
    Color color;
};

struct UVAttribute {
    glm::vec2 uv;
};

struct UVNormalAttribute {
    glm::vec2 uv;
    glm::vec3 normal;
};

// Core triangle template with abstract per-vertex attribute
template<typename TAttr = NoAttribute>
struct Triangle {
    glm::ivec2 v0, v1, v2;
    TAttr a0, a1, a2;
};

// Triangle type aliases
using PlainTriangle    = Triangle<>;
using ColoredTriangle  = Triangle<ColorAttribute>;
using TexturedTriangle = Triangle<UVAttribute>;
using UVNormalTriangle = Triangle<UVNormalAttribute>;


struct Point2Render {
    glm::ivec2 pointPos;
    Color color;
};



Color sampleTexture(SDL_Surface* texture, glm::vec2 uv);

#endif