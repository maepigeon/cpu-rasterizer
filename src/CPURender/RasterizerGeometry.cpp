#include "RasterizerGeometry.hpp"
#include <glm/glm.hpp>
#include <iostream>


Color sampleTexture(SDL_Surface* texture, glm::vec2 uv) {
    if (texture == nullptr) {
        // Use UV as color if no texture provided
        return Color{(uint8_t)(uv.x * 255), (uint8_t)(uv.y * 255), 0, 255};
    }
    // Reject uvs that are too big
    if (!std::isfinite(uv.x) || !std::isfinite(uv.y)) {
        std::cout << "Issue: a uv coordinate has a value that is NaN" << std::endl;
        return Color{0, 0, 0, 255};
    }
    float u = uv.x - std::floor(uv.x);
    float v = uv.y - std::floor(uv.y);

    int x = (int)(uv.x * texture->w) % texture->w;
    int y = (int)(uv.y * texture->h) % texture->h;
    if (x < 0) {
        x = 0; 
    } else if (x >= texture->w) {
        x = texture->w - 1;
    }
    if (y < 0) {
        y = 0; 
    } else if (y >= texture->h) {
        y = texture->h - 1;
    }
    
    uint32_t* pixels = (uint32_t*)texture->pixels;
    uint32_t pixel = pixels[y * texture->w + x];

    uint8_t r, g, b, a;
    SDL_GetRGBA(pixel, SDL_GetPixelFormatDetails(texture->format), nullptr, &r, &g, &b, &a);
    return Color{r, g, b, a};
}
