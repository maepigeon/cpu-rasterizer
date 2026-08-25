#ifndef RASTERIZER_GEOMETRY
#define RASTERIZER_GEOMETRY
#include <cstdint>

#include <glm/glm.hpp>
#include <SDL3/SDL.h>

typedef struct {
    glm::ivec2 v0;
    glm::ivec2 v1;
} Line;

typedef union {
    struct { std::uint8_t r, g, b, a; } channels;
    std::uint32_t bits;
} Color;

struct VariablePayload {
    static const int MAX_SIZE = 8;
    static const int NORMAL = 0;
    static const int UV = 3;
    static const int EXTRA = 5;
    float data[MAX_SIZE] = {};
};
static inline VariablePayload lerp(VariablePayload& a, VariablePayload& b, float t, int nValues) {
    VariablePayload outputData;
    for (int i = 0; i < nValues; i++) {
        outputData.data[i] = a.data[i] + t * (b.data[i] - a.data[i]);
    }
    return outputData;
}
static inline void setData(VariablePayload& payload, float data[VariablePayload::MAX_SIZE]) {
    for (int i = 0; i < VariablePayload::MAX_SIZE; i++) {
        payload.data[i] = data[i];
    }
}


// Attribute types
struct NoAttribute {};
// Core triangle template with abstract per-vertex attribute
template<typename TriangleAttribute = NoAttribute>
struct Triangle {
    glm::vec2 v0, v1, v2; // screen space positions
    float zNDC0, zNDC1, zNDC2;
    float perspectiveCorrection0, perspectiveCorrection1, perspectiveCorrection2;
    TriangleAttribute a0, a1, a2;
};

// Triangle types
using PlainTriangle = Triangle<NoAttribute>;
using ShadedTriangle = Triangle<VariablePayload>;


struct Point2Render {
    glm::ivec2 pointPos;
    Color color;
};

Color sampleTexture(SDL_Surface* texture, glm::vec2 uv);

#endif