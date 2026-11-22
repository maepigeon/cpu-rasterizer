#ifndef CPU_RASTERIZER_RENDER_MANAGER_HPP
#define CPU_RASTERIZER_RENDER_MANAGER_HPP

#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include  <rasterizer/CPURender/Rasterizer.hpp>
#include <rasterizer/Model.hpp>
#include <rasterizer/Mesh.hpp>
#include <rasterizer/IOEngine/Camera.hpp>
#include <rasterizer/ModelsManager.hpp>


namespace rasterizer {
    void model2stdout(std::vector<Mesh> meshes);

    class RenderManager {
    public:
        Rasterizer* getRasterizer() { return &cpuRasterizer; }
        RenderManager(SDL_Renderer* renderer, SDL_Window* window, Color color, int width, int height);
        void renderModel(ModelIndex modelIdx, ModelsManager& modelsManager, Camera* camera, double aspect);
        void destroy();
    private:
        Rasterizer cpuRasterizer;
    };
}
#endif