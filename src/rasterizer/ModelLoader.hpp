#ifndef CPU_RASTERIZER_MODEL_LOADER_HPP
#define CPU_RASTERIZER_MODEL_LOADER_HPP
#define TINYGLTF_ENABLE_BASE64
#include "tiny_gltf.h"
#include <utility>
#include "Model.hpp"

namespace rasterizer {
    class ModelLoader {
    public:
        Model loadFromGltf(tinygltf::Model& gltfModel);
    private:
        Mesh loadMesh(tinygltf::Model& gltfModel, tinygltf::Mesh& gltfMesh);
        uint8_t* getAttributePtr(tinygltf::Model& model, tinygltf::Accessor& accessor);
    };
}
#endif
