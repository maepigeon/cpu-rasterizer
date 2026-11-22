// Define these only in *one* .cpp file.
#ifndef TINYGLTF_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#endif
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif

#include <iostream>
#include <rasterizer/GLTFManager.hpp>
#include <filesystem>
#include <stdexcept>
using namespace tinygltf;
namespace fs = std::filesystem;
namespace rasterizer {
    // Loads a .glb or .gltf model into the GLTFManager as a tinygltf::model
    GLTFManager::ModelID GLTFManager::loadModel(std::string path) {
        std::unique_ptr<tinygltf::Model> model(new tinygltf::Model());
        TinyGLTF loader;
        std::string err;
        std::string warn;

        bool success = false;
        if (fs::path(path).extension()== ".glb") {
            // Note: model.get() returns a raw pointer (tinygltf::Model*)
            success = loader.LoadBinaryFromFile(model.get(), &err, &warn, path);
        }
        else {
            success = loader.LoadASCIIFromFile(model.get(), &err, &warn, path);
        }
        if (!warn.empty()) {
            printf("Warn: %s\n", warn.c_str());
        }

        if (!err.empty()) {
            printf("Err: %s\n", err.c_str());
        }
        if (!success) {
            printf("Failed to parse glTF: %s\n", path.c_str());
        }
        GLTFManager::ModelID id = nextID++;
        std::cout << "Loaded glTF model with:"
            << "\n - " << model->meshes.size() << " meshes"
            << "\n - " << model->materials.size() << " materials"
            << "\n - " << model->textures.size() << " textures"
            << "\n - " << model->images.size()<< " images"
            << "\n - " << model->animations.size() << " animations"
            << "\n - " << model->skins.size() << " skins"
            << "\n - " << model->cameras.size() << " cameras" 
            << "\n - " << model->lights.size() << " lights" 
            << std::endl;
        models.insert(std::make_pair(id, std::move(model)));
        return id;
    }

    SDL_Surface* GLTFManager::getTexture(GLTFManager::TextureID texId) {
        return textures[texId];
    }
    // still need to forgetTexture
    GLTFManager::TextureID GLTFManager::loadTextureFromGltf(tinygltf::Model& gltfModel, int imageIndex) {
        tinygltf::Image& image = gltfModel.images[imageIndex];
        // Copy pixels from texture in tinygltf memory into an SDL-owned surface
        SDL_Surface* surface = SDL_CreateSurface(image.width, image.height, SDL_PIXELFORMAT_RGBA32);
        memcpy(surface->pixels, image.image.data(), image.width * image.height * 4);
        textures.push_back(surface);
        return textures.size() - 1;
    }
    std::vector<GLTFManager::TextureID> GLTFManager::loadTexturesFromGltf(tinygltf::Model& gltfModel) {
        std::vector<GLTFManager::TextureID> result;
        for (int i = 0; i < gltfModel.images.size(); i++) {
            result.push_back(loadTextureFromGltf(gltfModel, i));
        }
        return result;
    }


    void GLTFManager::forgetTexture(GLTFManager::TextureID id) {
        SDL_DestroySurface(textures[id]);
        textures[id] = nullptr;
    }

    tinygltf::Model& GLTFManager::getGLTFModel(GLTFManager::ModelID id) {
        std::unordered_map<GLTFManager::ModelID,
        std::unique_ptr<tinygltf::Model>>::iterator it = models.find(id);
        if (it == models.end()) {
            throw std::runtime_error("Model not found: " + std::to_string(id)); 
        }
        return *(it->second); 
    }

    void GLTFManager::forgetModel(GLTFManager::ModelID id) {
        models.erase(id);
    } 
}