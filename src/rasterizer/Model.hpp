#ifndef CPU_RASTERIZER_MODEL_HPP
#define CPU_RASTERIZER_MODEL_HPP

#include <vector>
#include <rasterizer/Mesh.hpp>
#include <glm/glm.hpp> 
#include <stdint.h>
#include <tuple>

namespace rasterizer {
    using Texture = std::tuple<uint32_t /*w*/, uint32_t /*h*/, std::vector<glm::u8vec4> /*buffer*/>;
    class Model {
    public:
        std::vector<Mesh>& getMeshes();
        void addMesh(Mesh& mesh);
        bool empty();
        int nMeshes();
        Mesh* getMesh(uint32_t meshIndex);

        std::vector<Texture>& getTextures();
        Texture* getTexture(uint32_t textureIndex);
        int nTextures();
        Texture* createNewTexture();

        glm::mat4x4 worldTransform = glm::mat4x4(1.,0.,0.,0.,
                                                0.,1.,0.,0.,
                                                0.,0.,1.,0.,
                                                0.,0.,0.,1.);
    private:
        std::vector<Mesh> meshes;
        std::vector<Texture> textures; // For now, only one texture is supported per model, 
        // thus if there are multiple textured meshes they are treated as referring to the same texture.
        // TODO: Add multiple texture per model support with Mesh-to-Texture map 
    };
}
#endif