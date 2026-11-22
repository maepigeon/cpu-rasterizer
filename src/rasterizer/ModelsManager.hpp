#ifndef CPU_RASTERIZER_MODELS_MANAGER_HPP
#define CPU_RASTERIZER_MODELS_MANAGER_HPP

#include <rasterizer/Model.hpp>
#include <stdint.h>
#include <algorithm>
#include <vector>
#include <glm/glm.hpp> 
#include <map>
#include <glm/glm.hpp>

namespace rasterizer {
    using ModelIndex = uint32_t; // a model is given an integer value that is used to map to the model
    class ModelsManager {
        public:
            ModelIndex createNewModel();
            void addMeshToModel(Mesh mesh, ModelIndex idx);
            ModelIndex addModelToScene(Model model); 
            void deleteModelFromScene(ModelIndex idx);
            int getMeshCountFromModel(ModelIndex idx);
            Model& getModel(ModelIndex idx);
            std::pair<std::vector<glm::vec3>, std::vector<glm::ivec3>> getRawMeshTriangles(ModelIndex idx, int meshIndex);
            ModelIndex loadModelToSceneFromGLTF(std::string gltfPath);
            // Returns false if there is a mismatch between the size of vertexPositions and models[modelID]'s mesh @ meshIndex's vertex count
            bool updateMeshRawVertexPositions(std::vector<glm::vec3> vertexPositions, ModelIndex modelID, int meshIndex);
        private:
            int curModelIndex = 0;
            std::map<uint32_t/* index */, Model> models;  // The models in the scene, with int index
    };
}
#endif