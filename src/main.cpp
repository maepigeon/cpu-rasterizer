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

void displayHelp() {
    std::cout << "Execution failed..." << std::endl << std::endl;
    std::cout << "Usage: cpu-rasterizer width/w:width[Int] height/h:height[Int] model:model[String] Scale:scale[double]" << std::endl;
}

int main(int argc, char* argv[]) {
    
    std::unordered_map<std::string, std::string> args = parseArgs(argc, argv);
    int width  = args.count("width") ? std::stoi(args["width"]) : args.count("w") ? std::stoi(args["w"]) : 400;
    int height = args.count("height") ? std::stoi(args["height"]) : args.count("h") ? std::stoi(args["h"]) : 300;
    std::string modelPath = args.count("model") ? args["model"] : "../demo-scene/assets/Tetrahedron-uv.gltf";
    double scale = args.count("scale") ? std::stod(args["scale"]) : args.count("s") ? std::stod(args["s"]): 1.0;
    if (argc > 1) {
        std::string arg1 = argv[1];
        if (arg1 == "help") {
            displayHelp();
            return -1;
        }
    }
    int windowWidth  = (int)std::round(width  * scale);
    int windowHeight = (int)std::round(height * scale);

    SDL_Init(SDL_INIT_VIDEO);
    int64_t windowFlags = SDL_WINDOW_INPUT_FOCUS;
    SDL_Window* window = SDL_CreateWindow("Renderer", windowWidth, windowHeight, windowFlags);
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

    Color blackBackground = {0, 0, 0, 255};
    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, nullptr);
    RenderManager renderer(sdlRenderer, window, blackBackground, width, height);
    Camera camera;
    camera.setYawPitch(0.,0.);

    ResourceManager::TextureID texId = UINT32_MAX;
    SDL_Surface* texture = nullptr;
    if (!gltfModel.images.empty()) {
        texId = rm.loadTextureFromGltf(gltfModel);
        texture = rm.getTexture(texId);
    }

    // If scale > 1, create a texture to render into at low res, then upscale
    SDL_Texture* scaledTarget = nullptr;
    if (scale > 1) {
        scaledTarget = SDL_CreateTexture(
            sdlRenderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            width, height
            );
        if (!scaledTarget) {
            std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
        }
        // Nearest-neighbour (pixels stay crisp when scaled up)
        SDL_SetTextureScaleMode(scaledTarget, SDL_SCALEMODE_NEAREST);
    }

    double aspect = width / (float)height;

    // Main loop
    SDL_Event e;
    bool quit = false;
    uint32_t lastTime = SDL_GetTicks();
    while (!quit) {
        uint32_t currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f; // Convert ms to seconds
        lastTime = currentTime;
        
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;
            }
            camera.processSDLInputEvent(&e);
        }

        camera.update(deltaTime);
        if (scaledTarget) {
            // Render into the small texture
            SDL_SetRenderTarget(sdlRenderer, scaledTarget);
            renderer.renderModel(&model, &camera, aspect, texture);

            // Blit small texture to full window, scaled up
            SDL_SetRenderTarget(sdlRenderer, nullptr);
            SDL_FRect dst = { 0, 0, (float)windowWidth, (float)windowHeight };
            SDL_RenderTexture(sdlRenderer, scaledTarget, nullptr, &dst);
            SDL_RenderPresent(sdlRenderer);
        } else {
            renderer.renderModel(&model, &camera, aspect, texture);
        }
    }
    if (scaledTarget) {
        SDL_DestroyTexture(scaledTarget);
    }
    rm.forgetModel(id);
    if (texId != UINT32_MAX) rm.forgetTexture(texId);
    renderer.destroy();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}    
