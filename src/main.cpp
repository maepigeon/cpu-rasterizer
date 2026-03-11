#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <iostream>
#include <inttypes.h>

#include "CPURender/RasterizerGeometry.hpp"
#include "CPURender/RenderManager.hpp"
#include "Camera.hpp"
#include "ResourceManager.hpp"
#include "tiny_gltf.h"
#include "Model.hpp"
#include "ModelLoader.hpp"

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    int width = 640;
    int height = 480;
    int64_t windowFlags = SDL_WINDOW_INPUT_FOCUS;
    SDL_Window* window = SDL_CreateWindow("Rasten", width, height, windowFlags);
    
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    ResourceManager rm;
    ResourceManager::ModelID id = rm.loadModel("/Users/mae/workspace/graphics-projects/rasterizer/demo-scene/assets/Tetrahedron.gltf");
    tinygltf::Model& gltfModel = rm.getModel(id);
    ModelLoader modelLoader;
    Model model = modelLoader.loadFromGltf(gltfModel);
    model.worldTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));

    Color colorYellow = {255, 255, 0, 255};
    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, nullptr);
    RenderManager renderer(sdlRenderer, window, colorYellow, width, height);
    Camera camera;
    camera.setYawPitch(0.,0.);


    // Main loop
    SDL_Event e;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;
            }
            camera.processSDLInputEvent(&e);
            camera.update();
        }
        renderer.renderModel(&model, &camera);
    }
    rm.forgetModel(id);
    renderer.destroy();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}    
