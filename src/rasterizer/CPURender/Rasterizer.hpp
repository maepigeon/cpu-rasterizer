
#ifndef CPU_RASTERIZER_RASTERIZER_HPP
#define CPU_RASTERIZER_RASTERIZER_HPP

#include <vector> 
#include <glm/glm.hpp>
#include "RasterizerGeometry.hpp"
#include <SDL3/SDL.h>

namespace rasterizer {
struct TriSource {
    int meshIndex;  // which mesh in model->getMeshes()
    int triIndex;   // which triangle in that mesh (indices[triIndex*3 .. +2])
};
struct GeometryReference {
    int triangleIndex;
    float alpha, beta, gama; //barycentric coordinates of sample
    VertexData interpolatedTriangleData;
};
void bresenhamLine(std::vector<glm::ivec2>& points, int x0, int y0, int x1, int y1);
void renderTriangle(uint32_t* pixelBuffer, float* depthBuffer, GeometryReference* geometryBuffer, int triangleIndex, int width, int height, ShadedTriangle& tri, Texture* uvtexture);
class Rasterizer {
private:
    Texture* uvTexture = nullptr;
    SDL_Renderer* sdlRenderer = nullptr;
    SDL_Texture* outputTexture = nullptr;

    std::vector<float> depthBuffer;
    std::vector<GeometryReference> geometryBuffer;
    std::vector<TriSource> triSources;

    SDL_Surface* surface;
    int numTriangles = 0;
    int numLines = 0;
    std::vector<ShadedTriangle> triangles;
    std::vector<Edge> lines;
    ShadedTriangle t1, t2, t3;
    Edge line1;
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

    int width = 0, height = 0;
    Rasterizer();
    void initGeometryTest();
    void destroy();
    void update(GeometryReference* geometryBuffer);
    void update();
    void renderQueueInsert(ShadedTriangle tri, TriSource src);

    void clearRenderQueue();
    bool createCanvas(SDL_Renderer* renderer, SDL_Window* window, Color color, int width, int height);
    void setSurfaceColor(SDL_Surface *surface, int width, int height, Color color);
    void setTexture(Texture* tex);
};
}
#endif