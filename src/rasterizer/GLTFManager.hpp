#ifndef CPU_RASTERIZER_GLTF_MANAGER_HPP
#define CPU_RASTERIZER_GLTF_MANAGER_HPP

#include <unordered_map>
#include <memory>
#include "tiny_gltf.h"
#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include <utility>

namespace rasterizer {
    class GLTFManager { 
        public: 
            using ModelID = uint32_t;
            using TextureID = uint32_t;
            // Loads a model and returns a unique integer ID
            ModelID loadModel(std::string path);
            
            // Retrieve a model by ID
            tinygltf::Model& getGLTFModel(ModelID id);

            // Remove a model
            void forgetModel(ModelID id);

            SDL_Surface* getTexture(TextureID id);
            void forgetTexture(TextureID id);

            // Loads a texture and returns a unique integer ID for it
            TextureID loadTextureFromGltf(tinygltf::Model& gltfModel, int imageIndex = 0);
            std::vector<GLTFManager::TextureID> loadTexturesFromGltf(tinygltf::Model& gltfModel);
        private:
            std::unordered_map<ModelID, std::unique_ptr<tinygltf::Model>> models;
            ModelID nextID = 1;
            std::vector<SDL_Surface*> textures;
    };
}
#endif