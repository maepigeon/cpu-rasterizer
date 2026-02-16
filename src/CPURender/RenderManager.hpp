#ifndef RENDER_MANAGER
#define RENDER_MANAGER

#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include "Rasterizer.hpp"
#include "Model.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"

void model2stdout(std::vector<Mesh> meshes);

class RenderManager {
public:
    RenderManager(SDL_Renderer* renderer, SDL_Window* window, Color color, int width, int height);
    void renderModel(Model* model, Camera* camera);
    void destroy();
private:
    Rasterizer cpuRenderer;
    glm::vec2 clipToScreen(const glm::vec4 clip);
};

#endif