
#ifndef RASTERIZER
#define RASTERIZER

#include <vector> 
#include <glm/glm.hpp>
#include "RasterizerGeometry.hpp"
#include <SDL3/SDL.h>

struct TriSource {
    int meshIndex;  // which mesh in model->getMeshes()
    int triIndex;   // which triangle in that mesh (indices[triIndex*3 .. +2])
};
struct GeometryReference {
    int triangleIndex;
    float alpha, beta, gama; //barycentric coordinates of sample
    VariablePayload interpolatedTriangleData;
};
void bresenhamLine(std::vector<glm::ivec2>& points, int x0, int y0, int x1, int y1);
void renderTriangle(uint32_t* pixelBuffer, float* depthBuffer, GeometryReference* geometryBuffer, int triangleIndex, int width, int height, ShadedTriangle& tri, SDL_Surface* texture);
class Rasterizer {
private:
    SDL_Surface* uvTexture;
    SDL_Renderer*  sdlRenderer;
    SDL_Texture* texture;

    std::vector<float> depthBuffer;
    std::vector<GeometryReference> geometryBuffer;
    std::vector<TriSource> triSources;


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

    float* getDepthBuffer() {
        return depthBuffer.data();
    }
    GeometryReference* getGeometryBuffer() {
        return geometryBuffer.data();
    }
    TriSource getTriSource(int queueIndex) { return triSources[queueIndex]; }

    int width, height;
    Rasterizer();
    void initGeometryTest();
    float wedge2(glm::vec2 a, glm::vec2 b);
    void destroy();
    void update(GeometryReference* geometryBuffer);
    void update();
    void renderQueueInsert(ShadedTriangle tri, TriSource src);

    void clearRenderQueue();
    bool createCanvas(SDL_Renderer* renderer, SDL_Window* window, Color color, int width, int height);
    void setSurfaceColor(SDL_Surface *surface, int width, int height, Color color);
    void setTexture(SDL_Surface* tex);
};
#endif