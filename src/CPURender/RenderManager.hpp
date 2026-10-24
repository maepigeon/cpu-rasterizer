#ifndef RENDER_MANAGER
#define RENDER_MANAGER

#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include "Rasterizer.hpp"
#include "Model.hpp"
#include "Mesh.hpp"
#include "IOEngine/Camera.hpp"

void model2stdout(std::vector<Mesh> meshes);

class RenderManager {
public:
    Rasterizer* getRasterizer() { return &cpuRasterizer; }
    RenderManager(SDL_Renderer* renderer, SDL_Window* window, Color color, int width, int height);
    void renderModel(Model* model, Camera* camera, double aspect, SDL_Surface* texture);
    void destroy();
private:
    Rasterizer cpuRasterizer;
};

#endif