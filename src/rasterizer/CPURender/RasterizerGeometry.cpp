#include "RasterizerGeometry.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include <tuple>

namespace rasterizer {

Color sampleTexture(Texture* texture, glm::vec2 uv) {
    if (texture == nullptr) {
        // Use UV as color if no texture provided
        return Color{(uint8_t)(uv.x * 255), (uint8_t)(uv.y * 255), 0, 255};
    }
    int w = std::get<0>(*texture); int h = std::get<1>(*texture);
    // Reject uvs that are too big
    if (!std::isfinite(uv.x) || !std::isfinite(uv.y)) {
        std::cout << "Issue: a uv coordinate has a value that is NaN" << std::endl;
        return Color{0, 0, 0, 255};
    }
    float u = uv.x - std::floor(uv.x);
    float v = uv.y - std::floor(uv.y);

    int x = (int)(uv.x * w) % w;
    int y = (int)(uv.y * h) % h;
    if (x < 0) {
        x = 0; 
    } else if (x >= w) {
        x = w - 1;
    }
    if (y < 0) {
        y = 0; 
    } else if (y >= h) {
        y = h - 1;
    }
    
    std::vector<glm::u8vec4>& pixels = std::get<2>(*texture);
    glm::u8vec4 pixel = pixels[y * w + x];
    return Color{pixel.r, pixel.g, pixel.b, pixel.a};
}

void setData(VertexData payload, float data[VertexData::MAX_SIZE]) {
    for (int i = 0; i < VertexData::MAX_SIZE; i++) {
        payload.data[i] = data[i];
    }
}

VertexData lerp(VertexData a, VertexData b, float t, int nValues) {
    VertexData outputData;
    for (int i = 0; i < nValues; i++) {
        outputData.data[i] = a.data[i] + t * (b.data[i] - a.data[i]);
    }
    return outputData;
}
}