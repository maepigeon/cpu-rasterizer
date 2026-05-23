
#ifndef RASTERIZER
#define RASTERIZER

#include <vector> 
#include <glm/glm.hpp>
#include "RasterizerGeometry.hpp"
#include <SDL3/SDL.h>
void bresenhamLine(std::vector<glm::ivec2>& points, int x0, int y0, int x1, int y1);
void renderTriangle(std::vector<Point2Render>& points, glm::ivec2 A, glm::vec2 uvA,
                    glm::ivec2 B, glm::vec2 uvB,       glm::ivec2 C, glm::vec2 uvC,
                    SDL_Surface* texture);

class Rasterizer {
private:
    SDL_Surface* uvTexture;

    SDL_Renderer* sdlRenderer;
    SDL_Texture* texture;
    SDL_Surface* surface;
    int numTriangles;
    int numLines;
    //std::vector<Triangle<UVAttribute>> triangles;
    std::vector<PlainTriangle> triangles;
    std::vector<Line> lines;
    //Triangle<UVAttribute> t1, t2, t3;
    PlainTriangle t1, t2, t3;
    Line line1;
    std::vector<glm::ivec2> lineABPoints;
    Color color;

public:
    struct RasterBuffer
    {
        Color* pixels = nullptr;
        std::uint32_t width = 0;
        std::uint32_t height = 0;
    };
    int width, height;
    Rasterizer();
    void initGeometry();
    float wedge2(glm::vec2 a, glm::vec2 b);
    void destroy();
    void RenderTriangle();
    void update();
    void renderQueueInsert(PlainTriangle tri);
    //void renderQueueInsert(Triangle<UVAttribute> tri);
    void clearRenderQueue();
    bool createCanvas(SDL_Renderer* renderer, SDL_Window* window, Color color, int width, int height);
    void setSurfaceColor(SDL_Surface *surface, int width, int height, Color color);
    void setPixel(SDL_Surface *surface, int x, int y, Color color);
    void setTexture(SDL_Surface* tex);
};
#endif