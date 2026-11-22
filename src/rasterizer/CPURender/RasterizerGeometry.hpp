#ifndef CPU_RASTERIZER_RASTERIZER_GEOMETRY_HPP
#define CPU_RASTERIZER_RASTERIZER_GEOMETRY_HPP
#include <cstdint>

#include <glm/glm.hpp>
#include <SDL3/SDL.h>

#include <rasterizer/Model.hpp>


namespace rasterizer {
    typedef struct {
        glm::ivec2 v0;
        glm::ivec2 v1;
    } Edge;

    typedef union {
        struct { 
            std::uint8_t r, g, b, a; 
        } channels;
        std::uint32_t bits;
    } Color;

    // Stores extra data to be used in rasterization
    struct VertexData {
        static const int MAX_SIZE = 8;
        static const int NORMAL = 0;
        static const int UV = 3;
        static const int EXTRA = 5;
        float data[MAX_SIZE] = {};
    };

    VertexData lerp(VertexData a, VertexData b, float t, int nValues);
    void setData(VertexData payload, float data[]);


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
    using ShadedTriangle = Triangle<VertexData>;


    struct Point2Render {
        glm::ivec2 pointPos;
        Color color;
    };

    Color sampleTexture(Texture* texture, glm::vec2 uv);
}
#endif