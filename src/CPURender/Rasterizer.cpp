#include <glm/glm.hpp>
#include <iostream>

#include "RasterizerGeometry.hpp"
#include "Rasterizer.hpp"

Rasterizer::Rasterizer() {}

void Rasterizer::initGeometry() {
    // Initialize the geometry
    numTriangles = 3;
    /*t1 = {{0.0, 0.0}, {0.0, 100.0}, {100.0, 0.0}};
    t2 = {{100.0, 200.0}, {100.0, 0.0}, {200.0, 50.0}};
    t3 = {{300.0, 200.0}, {350.0, 400.0}, {350.0, 250.0}};
    triangles.push_back(t1);
    triangles.push_back(t2);
    triangles.push_back(t3);*/
    line1 = {{200.0, 60.0}, {100 , 260}};
    lines.push_back(line1);
}

void Rasterizer::renderQueueInsert(Triangle tri) {
    triangles.push_back(tri);
    numTriangles++;
}
void Rasterizer::clearRenderQueue() {
    triangles.clear();
    lines.clear();
    lineABPoints.clear();
    numLines = 0;
    numTriangles = 0;
}

void Rasterizer::destroy() {
    SDL_DestroySurface(surface);
    SDL_DestroyTexture(texture);
}

void Rasterizer::RenderTriangle() {
    // Use early-z bufferiing
    // 1. Triangle setup -> (edge equations, data setup)
    // 2. Triangle traversal -> Generate fragment properties using data interpolated from vertices. Perspective correct interpolation. Send pixels to pixel processing state   
    // 3. Run fragment shader per pixel
    // 4. Merging state (using z-buffer)
}

void Rasterizer::update() {

    //std::cout << "Rendering " << numTriangles << " tris" << std::endl;
    lineABPoints.clear();

    SDL_RenderClear(sdlRenderer);
    Color colorRed= {255, 0, 0, 255};
    Color colorBlue= {0, 0, 255, 255};
    //Clear the renderer
    setSurfaceColor(surface, width, height, color);
    //Render Lines
    for (int i = 0; i < numLines; i++) {
        bresenhamLine(lineABPoints, lines[i].v0.x, lines[i].v0.y, lines[i].v1.x, lines[i].v1.y); 
    }
    for (int i = 0; i < lineABPoints.size(); i++) {
        setPixel(surface, (float)lineABPoints[i].x, (float)lineABPoints[i].y, colorRed);
    }
    // Render all triangles, in order defined in the list
    for (int i = 0; i < numTriangles; i++) {
        std::vector<Point2Render> points; // The points to render
        renderTriangle(points, triangles[i].v0, triangles[i].v1, triangles[i].v2);
        Color color = colorBlue;
        if (i == 1) {
            color = colorBlue;
        }
        for (int i = 0; i < points.size(); i++) {
            setPixel(surface, points[i].pointPos.x, points[i].pointPos.y, points[i].color);
        }
    }
    SDL_UpdateTexture(texture, nullptr, surface->pixels, surface->pitch);
    SDL_RenderTexture(sdlRenderer, texture, NULL, NULL);
    SDL_RenderPresent(sdlRenderer);
}


bool Rasterizer::createCanvas(SDL_Renderer* renderer, SDL_Window* window, Color color, int width, int height) {
    this->width = width;
    this->height = height;
    this->color = color;
    sdlRenderer = renderer;
    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
  
    // Fill the window with a solid color
    surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);
    setSurfaceColor(surface, width, height, color);
    // Create a texture that can be sent to the gpu from the (cpu-owned) surface 
    texture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, width, height );

    return true;
}

void Rasterizer::setSurfaceColor(SDL_Surface *surface, int width, int height, Color color) {
    SDL_LockSurface(surface);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            setPixel(surface, x, y, color);
        }
    }
    SDL_UnlockSurface(surface);
}


// https://stackoverflow.com/questions/20070155/how-to-set-a-pixel-in-a-sdl-surface
void Rasterizer::setPixel(SDL_Surface *surface, int x, int y, Color color)
{
    if (x < 0 || x >= width || y < 0 || y >= height) {
        //std::cout << "Pixel out of bounds!" << std::endl;
        return;
    }
    uint32_t* const target_pixel = (uint32_t *) ((uint8_t *) surface->pixels
                + y * surface->pitch + x * SDL_BYTESPERPIXEL(surface->format));
    *target_pixel = color.bits;
}

void bresenhamLineH(std::vector<glm::ivec2>& points, int x0, int y0, int x1, int y1) {
    if (x0 > x1) { // Swap points 1 and 2
        int tmp = x0;
        x0 = x1;
        x1 = tmp;
        tmp = y0;
        y0 = y1;
        y1 = tmp;
    }
    int dx = x1 - x0;
    int dy = y1 - y0;

    int dir = (dy < 0) ? -1 : 1;
    dy *= dir;
    
    if (dx != 0) {
        int y = y0;
        int p = 2 * dy - dx;
        for (int i = 0; i <= dx; i++) {
            points.push_back({x0 + i, y});
            if (p >= 0) {
                y += dir;
                p = p - 2*dx;
            }
            p = p + 2*dy;
        }
    }
}

float wedge2(glm::vec2 a, glm::vec2 b) {
    return a.x * b.y - a.y * b.x;
}

void bresenhamLineV(std::vector<glm::ivec2>& points, int x0, int y0, int x1, int y1) {
    if (y0 > y1) { // Swap points 1 and 2
        int tmp = x0;
        x0 = x1;
        x1 = tmp;
        tmp = y0;
        y0 = y1;
        y1 = tmp;
    }
    int dx = x1 - x0;
    int dy = y1 - y0;

    int dir = (dx < 0) ? -1 : 1;
    dx *= dir;
    
    if (dy != 0) {
        int x = x0;
        int p = 2 * dx - dy;
        for (int i = 0; i <= dy; i++) {
            points.push_back({x, y0 + i});
            if (p >= 0) {
                x += dir;
                p = p - 2*dy;
            }
            p = p + 2*dx;
        }
    }
}

// Used this video to learn Bresenham's line algorithm https://youtu.be/CceepU1vIKo?si=9oqYiaLDjKdF_4-t
void bresenhamLine(std::vector<glm::ivec2>& points, int x0, int y0, int x1, int y1) {
    if (abs(x1 - x0) > abs(y1-y0)) {
        return bresenhamLineH(points, x0, y0, x1, y1);
    } else {
        return bresenhamLineV(points, x0, y0, x1, y1);
    }
}

void updateLREdgesH(int* LPoints, int* RPoints, int minY, int x0, int y0, int x1, int y1) {
    if (x0 > x1) { // Swap points 1 and 2
        int tmp = x0;
        x0 = x1;
        x1 = tmp;
        tmp = y0;
        y0 = y1;
        y1 = tmp;
    }
    int dx = x1 - x0;
    int dy = y1 - y0;

    int dir = (dy < 0) ? -1 : 1;
    dy *= dir;
    
    if (dx != 0) {
        int y = y0;
        int p = 2 * dy - dx;
        for (int i = 0; i <= dx; i++) {
            int yIdx = y - minY;
            if (x0 + i < LPoints[yIdx]) {
                LPoints[yIdx] = x0 + i;
            }            
            if (x0 + i > RPoints[yIdx]) {
                RPoints[yIdx] = x0 + i;
            }
            if (p >= 0) {
                y += dir;
                p = p - 2*dx;
            }
            p = p + 2*dy;
        }
    }
}
void updateLREdgesV(int* LPoints, int* RPoints, int minY, int x0, int y0, int x1, int y1) {

    if (y0 > y1) { // Swap points 1 and 2
        int tmp = x0;
        x0 = x1;
        x1 = tmp;
        tmp = y0;
        y0 = y1;
        y1 = tmp;
    }
    int dx = x1 - x0;
    int dy = y1 - y0;

    int dir = (dx < 0) ? -1 : 1;
    dx *= dir;
    
    if (dy != 0) {
        int x = x0;
        int p = 2 * dx - dy;
        for (int i = 0; i <= dy; i++) {
            int yIdx = y0 + i - minY;
            if (x < LPoints[yIdx]) {
                LPoints[yIdx] = x;
            }            
            if (x > RPoints[yIdx]) {
                RPoints[yIdx] = x;
            }
            if (p >= 0) {
                x += dir;
                p = p - 2*dy;
            }
            p = p + 2*dx;
        }
    }
}

// Uses bresenham's line algorithm to generate an array of the leftmost and rightmost points in the triangle for each row
void getTriangleMaxMinArrays(int* LPointsArray, int* RPointsArray, int minY, int x0, int y0, int x1, int y1) {
    if (abs(x1 - x0) > abs(y1-y0)) {
        return updateLREdgesH(LPointsArray, RPointsArray, minY, x0, y0, x1, y1);
    } else {
        return updateLREdgesV(LPointsArray, RPointsArray, minY, x0, y0, x1, y1);
    }
}

// A is left-part of base, B is right-part of base, C is the point
void renderTriangle(std::vector<Point2Render>& points, glm::ivec2 A, glm::ivec2 B, glm::ivec2 C) {
    int maxY = std::max(std::max(A.y, B.y), C.y);
    int minY = std::min(std::min(A.y, B.y), C.y);
    int triHeight = maxY - minY + 1;
    int leftPoints[triHeight];
    int rightPoints[triHeight]; 
    std::fill_n(leftPoints, triHeight, 1000000);
    std::fill_n(rightPoints, triHeight, -1000000);
    getTriangleMaxMinArrays(leftPoints, rightPoints, minY, A.x, A.y, B.x, B.y);
    getTriangleMaxMinArrays(leftPoints, rightPoints, minY, B.x, B.y, C.x, C.y);
    getTriangleMaxMinArrays(leftPoints, rightPoints, minY, C.x, C.y, A.x, A.y);
    for (int y = 0; y < triHeight; y++) {
        int lx = leftPoints[y];
        int rx = rightPoints[y];
        // Draw a line from lx to rx
        for (int x = lx; x <= rx; x++) {
            // Area of triangle ABC
            glm::ivec2 X = {x, y + minY};
            float _2A_abc = abs(wedge2((A - B), (C - B)));
            float _2A_xab = abs(wedge2((X - A), (X - B)));
            float _2A_xbc = abs(wedge2((X - B), (X - C)));
            float _2A_xca = abs(wedge2((X - C), (X - A)));
            float alpha = _2A_xbc / _2A_abc;
            float beta  = _2A_xca / _2A_abc;
            float gamma = 1.0 - alpha - beta;

            Color color = {(uint8_t)(alpha * 255), (uint8_t)(beta * 255), (uint8_t)(gamma * 255), 255};
            points.push_back({{x, y + minY}, color});
        }
    } 
}
