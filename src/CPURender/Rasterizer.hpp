
#ifndef RASTERIZER
#define RASTERIZER

#include <vector> 
#include <glm/glm.hpp>
#include "RasterizerGeometry.hpp"
#include <SDL3/SDL.h>
void bresenhamLine(std::vector<glm::ivec2>& points, int x0, int y0, int x1, int y1);
void renderTriangle(uint32_t* pixelBuffer, float* depthBuffer, int width, int height, ShadedTriangle& tri, SDL_Surface* texture);
class Rasterizer {
private:
    SDL_Surface* uvTexture;
    SDL_Renderer* sdlRenderer;
    SDL_Texture* texture;
    float* depthBuffer;
    SDL_Surface* surface;
    int numTriangles;
    int numLines;
    std::vector<ShadedTriangle> triangles;
    std::vector<Line> lines;
    ShadedTriangle t1, t2, t3;
    Line line1;
    std::vector<glm::ivec2> lineABPoints;
    Color color;

public:
    struct RasterBuffer {
        Color* pixels = nullptr;
        std::uint32_t width = 0;
        std::uint32_t height = 0;
    };

    int width, height;
    Rasterizer();
    void initGeometryTest();
    float wedge2(glm::vec2 a, glm::vec2 b);
    void destroy();
    void update();
    void renderQueueInsert(ShadedTriangle tri);
    void clearRenderQueue();
    bool createCanvas(SDL_Renderer* renderer, SDL_Window* window, Color color, int width, int height);
    void setSurfaceColor(SDL_Surface *surface, int width, int height, Color color);
    void setTexture(SDL_Surface* tex);
};
#endif