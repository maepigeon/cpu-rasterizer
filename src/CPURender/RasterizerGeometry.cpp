#include "RasterizerGeometry.hpp"
#include <glm/glm.hpp>


Color sampleTexture(SDL_Surface* texture, glm::vec2 uv) {
    if (texture == nullptr) {
        // Use UV as color if no texture provided
        return Color{(uint8_t)(uv.x * 255), (uint8_t)(uv.y * 255), 0, 255};
    }

    int x = (int)(uv.x * texture->w) % texture->w;
    int y = (int)(uv.y * texture->h) % texture->h;
    
    uint32_t* pixels = (uint32_t*)texture->pixels;
    uint32_t pixel = pixels[y * texture->w + x];

    uint8_t r, g, b, a;
    SDL_GetRGBA(pixel, SDL_GetPixelFormatDetails(texture->format), nullptr, &r, &g, &b, &a);
    return Color{r, g, b, a};
}
