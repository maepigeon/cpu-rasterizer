#include <rasterizer/Model.hpp>
#include <rasterizer/GLTFManager.hpp>
namespace rasterizer {
    std::vector<Mesh>& Model::getMeshes() {
        return meshes;
    }
    void Model::addMesh(Mesh& mesh) {
        meshes.push_back(mesh);
    }
    int Model::nMeshes() {
        return meshes.size();
    }
    bool Model::empty() {
        return meshes.empty();
    }
    Mesh* Model::getMesh(uint32_t meshIndex) {
        return &meshes[meshIndex];
    }
    std::vector<Texture>& Model::getTextures() {
        return textures;
    }
    Texture* Model::getTexture(uint32_t textureIndex) {
        return &textures[textureIndex];
    }
    int Model::nTextures() {
        return textures.size();
    }
    Texture* Model::createNewTexture() {
        Texture newTexture;
        textures.push_back(newTexture);
        return &textures[nTextures() - 1];
    }
}