#ifndef MODEL_LOADER
#define MODEL_LOADER

#include "Model.hpp"
#define TINYGLTF_ENABLE_BASE64
#include "tiny_gltf.h"

class ModelLoader {
public:
    Model loadFromGltf(tinygltf::Model& gltfModel);

private:
    Mesh loadMesh(tinygltf::Model& gltfModel, tinygltf::Mesh& gltfMesh);
    uint8_t* getAttributePtr(tinygltf::Model& model, tinygltf::Accessor& accessor);
};
#endif
