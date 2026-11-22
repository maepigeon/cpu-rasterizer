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
    int Model::createNewMesh(std::vector<glm::vec3>& vertices, std::vector<glm::ivec3>& indices) {
        Mesh m;
        m.indices.resize(indices.size() * 3);
        for (int i = 0; i < indices.size(); i++) {
            m.indices[i * 3] = indices[i].x;
            m.indices[i * 3 + 1] = indices[i].y;
            m.indices[i * 3 + 2] = indices[i].z;
        }
        m.vertices.resize(vertices.size());
        for (int i = 0; i < vertices.size(); i++) {
            Mesh::Vertex newVertex = {vertices[i], {}, {}, {}};
            m.vertices[i] = newVertex;
        }
        meshes.push_back(m);
        return nMeshes();
    }
}