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
#include <time.h>

std::unordered_map<std::string, std::string> parseArgs(int argc, char* argv[]) {
    std::unordered_map<std::string, std::string> args;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        size_t colon = arg.find(':');
        if (colon != std::string::npos) {
            std::string key = arg.substr(0, colon);
            std::string value = arg.substr(colon + 1);
            args[key] = value;
        }
    }
    return args;
}



int main(int argc, char* argv[]) {
    std::unordered_map<std::string, std::string> args = parseArgs(argc, argv);
    int width  = args.count("width")  ? std::stoi(args["width"])  :
                 args.count("w")     ? std::stoi(args["w"])     : 2000;
    int height = args.count("height") ? std::stoi(args["height"]) :
                 args.count("h")     ? std::stoi(args["h"])     : 1200;
    std::string modelPath = args.count("model") ? args["model"] : "../demo-scene/assets/Triangle.gltf";


    SDL_Init(SDL_INIT_VIDEO);
    int64_t windowFlags = SDL_WINDOW_INPUT_FOCUS;
    SDL_Window* window = SDL_CreateWindow("Renderer", width, height, windowFlags);
    SDL_SetWindowRelativeMouseMode(window, true);
    
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    ResourceManager rm;
    ResourceManager::ModelID id = rm.loadModel(modelPath);
    tinygltf::Model& gltfModel = rm.getModel(id);
    ModelLoader modelLoader;
    Model model = modelLoader.loadFromGltf(gltfModel);
    model.worldTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));

    Color colorYellow = {255, 255, 0, 255};
    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, nullptr);
    RenderManager renderer(sdlRenderer, window, colorYellow, width, height);
    Camera camera;
    camera.setYawPitch(0.,0.);

    ResourceManager::TextureID texId = UINT32_MAX;
    SDL_Surface* texture = nullptr;
    if (!gltfModel.images.empty()) {
        texId = rm.loadTextureFromGltf(gltfModel);
        texture = rm.getTexture(texId);
    }



    // Main loop
    SDL_Event e;
    bool quit = false;
    clock_t time = clock();
    while (!quit) {
        clock_t newTime = clock();
        clock_t deltaTime = newTime - time;
        time = newTime;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;
            }
            camera.processSDLInputEvent(&e);
            camera.update(deltaTime * 0.000001f); // time in ns, divide by a million for seconds
        }
        renderer.renderModel(&model, &camera, texture);
    }
    rm.forgetModel(id);
    if (texId != UINT32_MAX) rm.forgetTexture(texId);
    renderer.destroy();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}    
