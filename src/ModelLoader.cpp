#define TINYGLTF_ENABLE_BASE64
#include "ModelLoader.hpp"
#include <stdexcept>
#include <cstring>
#include <iostream>

Model ModelLoader::loadFromGltf(tinygltf::Model& gltfModel)
{
    Model model;
    std::vector<tinygltf::Mesh>& gltfMeshes = gltfModel.meshes;
    int meshCount = gltfMeshes.size();
    for (size_t i = 0; i < meshCount; i++) {
        tinygltf::Mesh& gltfMesh = gltfMeshes[i];
        Mesh mesh = loadMesh(gltfModel, gltfMesh);
        model.addMesh(mesh);
    }
    return model;
}

Mesh ModelLoader::loadMesh(tinygltf::Model& gltfModel, tinygltf::Mesh& gltfMesh)
{
    Mesh mesh;

    int primitiveCount = (int)gltfMesh.primitives.size();

    for (int p = 0; p < primitiveCount; p++) {
        tinygltf::Primitive& primitive = gltfMesh.primitives[p];

        // POSITION
        std::map<std::string, int>& attributes = primitive.attributes;

        auto posIt = attributes.find("POSITION");
        if (posIt == attributes.end()) {
            throw std::runtime_error("Mesh missing POSITION attribute");
        }

        tinygltf::Accessor& posAccessor = gltfModel.accessors[posIt->second];
        uint8_t* posBase = getAttributePtr(gltfModel, posAccessor);
        int posStride = (int)posAccessor.ByteStride(gltfModel.bufferViews[posAccessor.bufferView]);
        if (posStride == 0)
            posStride = tinygltf::GetNumComponentsInType(posAccessor.type) *
                        tinygltf::GetComponentSizeInBytes(posAccessor.componentType);

        // NORMAL
        auto normalIterator = attributes.find("NORMAL");
        bool hasNormals = (normalIterator != attributes.end());
        tinygltf::Accessor* normalAccessorPtr = nullptr;
        uint8_t* normalBase = nullptr;
        int normalStride = 0;

        if (hasNormals) {
            normalAccessorPtr = &gltfModel.accessors[normalIterator->second];
            normalBase = getAttributePtr(gltfModel, *normalAccessorPtr);
            normalStride = (int)normalAccessorPtr->ByteStride(gltfModel.bufferViews[normalAccessorPtr->bufferView]);
            if (normalStride == 0)
                normalStride = tinygltf::GetNumComponentsInType(normalAccessorPtr->type) *
                               tinygltf::GetComponentSizeInBytes(normalAccessorPtr->componentType);
        }

        // TEXCOORD_0 
        auto uvIterator = attributes.find("TEXCOORD_0");
        bool hasUVs = (uvIterator != attributes.end());
        tinygltf::Accessor* uvAccessorPtr = nullptr;
        uint8_t* uvBase = nullptr;
        int uvStride = 0;

        if (hasUVs) {
            uvAccessorPtr = &gltfModel.accessors[uvIterator->second];
            uvBase = getAttributePtr(gltfModel, *uvAccessorPtr);
            uvStride = (int)uvAccessorPtr->ByteStride(gltfModel.bufferViews[uvAccessorPtr->bufferView]);
            if (uvStride == 0)
                uvStride = tinygltf::GetNumComponentsInType(uvAccessorPtr->type) *
                           tinygltf::GetComponentSizeInBytes(uvAccessorPtr->componentType);
        }

        // Allocate vertices
        int vertexCount = (int)posAccessor.count;
        std::cout << "Vertex count: " << vertexCount << std::endl;
        mesh.vertices.resize(vertexCount);

        for (int i = 0; i < vertexCount; i++) {
            // POSITION
            glm::vec3* posPtr = (glm::vec3*)(posBase + i * posStride);
            mesh.vertices[i].position = *posPtr;

            // NORMAL or default
            if (hasNormals) {
                glm::vec3* norPtr = (glm::vec3*)(normalBase + i * normalStride);
                mesh.vertices[i].normal = *norPtr;
            } else {
                mesh.vertices[i].normal = glm::vec3(0.0f, 0.0f, 1.0f);
            }

            // UV or default
            if (hasUVs) {
                glm::vec2* uvPtr = (glm::vec2*)(uvBase + i * uvStride);
                mesh.vertices[i].texcoord = *uvPtr;
            } else {
                mesh.vertices[i].texcoord = glm::vec2(0.0f, 0.0f);
            }
        }

        // Indices 
        tinygltf::Accessor& idxAccessor = gltfModel.accessors[primitive.indices];
        uint8_t* idxBase = getAttributePtr(gltfModel, idxAccessor);
        int idxCount = (int)idxAccessor.count;
        std::cout << "Index count: " << idxCount << std::endl;

        mesh.indices.resize(idxCount);
        if (idxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
            uint16_t* src = (uint16_t*)idxBase;
            for (int i = 0; i < idxCount; i++) {
                mesh.indices[i] = (uint32_t)src[i];
            }
        }
        else if (idxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
            uint32_t* src = (uint32_t*)idxBase;
            for (int i = 0; i < idxCount; i++) {
                mesh.indices[i] = src[i];
            }
        }
        else {
            throw std::runtime_error("Unsupported index type");
        }
    }
    return mesh;
}

uint8_t* ModelLoader::getAttributePtr(tinygltf::Model& model, tinygltf::Accessor& accessor)
{
    tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
    tinygltf::Buffer& buffer = model.buffers[view.buffer];
    int offset = (int)view.byteOffset + (int)accessor.byteOffset;
    return buffer.data.data() + offset;
}
