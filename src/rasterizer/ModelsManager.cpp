#include <rasterizer/ModelsManager.hpp>
#include <rasterizer/GLTFManager.hpp>
#include <rasterizer/ModelLoader.hpp>
#include <tuple>
#include <iostream>

namespace rasterizer {
    ModelIndex ModelsManager::createNewModel() {        
        Model m;
        models.insert({curModelIndex, m});
        curModelIndex++;
        return curModelIndex - 1;
    }
    void ModelsManager::addMeshToModel(Mesh mesh, ModelIndex idx) {
        models[idx].addMesh(mesh);
    }
    ModelIndex ModelsManager::addModelToScene(Model model) {
        models.insert({curModelIndex, model});
        curModelIndex++;
        return curModelIndex - 1;
    }
    void ModelsManager::deleteModelFromScene(ModelIndex idx) {
        models.erase(idx);
    }
    int ModelsManager::getMeshCountFromModel(ModelIndex idx) {
        return models[idx].getMeshes().size();
    }
    Model& ModelsManager::getModel(ModelIndex idx) {
        return models[idx];
    }
    // returns the index of the newly created mesh in the model specified by modelIndex
    int ModelsManager::createMeshFromRawTriangles(std::vector<glm::vec3> vertices, std::vector<uint32_t> indices, ModelIndex idx) {
        Model& model = models[idx];
        model.createNewMesh(vertices, indices);
        return model.nMeshes();
    }
    std::pair<std::vector<glm::vec3>, std::vector<glm::ivec3>> ModelsManager::getRawMeshTriangles(ModelIndex idx, int meshIndex) {
        Mesh mesh = models[idx].getMeshes()[meshIndex];
        mesh.buildWeldMap(1e-6);
        return {mesh.weldedPositions, mesh.getWeldedTriangles()};
    }
    ModelIndex ModelsManager::loadModelToSceneFromGLTF(std::string gltfPath) {
        // Get gltf model
        GLTFManager gm;
        GLTFManager::ModelID gltfID = gm.loadModel(gltfPath);        
        // Convert that to a in-engine model to put in the scene
        ModelLoader ml;
        tinygltf::Model gltfModel = gm.getGLTFModel(gltfID);
        ModelIndex modelIndex = addModelToScene(ml.loadFromGltf(gltfModel));
        Model& model = models[modelIndex];

        // Get texture(s) from the gltf model and open the texture files
        if (!gltfModel.images.empty()) {
            std::vector<rasterizer::GLTFManager::TextureID> gltfTexturesID = gm.loadTexturesFromGltf(gltfModel);
            for (int i = 0; i < gltfTexturesID.size(); i++) {
                SDL_Surface* sdlTex = gm.getTexture(gltfTexturesID[i]);
                
                // Go from SDL surface intermediary from gltf initial load to model's Texture
                Texture* tex = model.createNewTexture();
                std::cout << "ADDED A TEXTURE" << std::endl;
                int w = sdlTex->w; int h = sdlTex->h;
                std::get<0>(*tex) = w; std::get<1>(*tex) = h;
                std::get<2>(*tex).resize(w * h);
                uint32_t nPixels = w * h;
                std::vector<glm::u8vec4>& pixels = std::get<2>(*tex);
                SDL_PixelFormat format = sdlTex->format;
                switch (format) {
                    case SDL_PIXELFORMAT_RGBA8888:
                    case SDL_PIXELFORMAT_RGBA32:
                        for (int p = 0; p < nPixels; p++) {
                            glm::u8vec4 pix;
                            SDL_ReadSurfacePixel(sdlTex, p % w, p / w, &pix[0], &pix[1], &pix[2], &pix[3]);
                            pixels[p] = pix;
                        }
                        break;
                    default:
                        std::cout << "Pixelformat " << format << " of sdltexture on model, index " << modelIndex << " is not supported. Only SDL_PIXELFORMAT_RGBA32 is supported\n";
                        break;
                }
            }
            // Start cleanup
            for (int i = 0; i < gltfTexturesID.size(); i++) {
                gm.forgetTexture(gltfTexturesID[i]);
            }
        }
        // end of cleanup
        gm.forgetModel(gltfID);
        return modelIndex;
    }
    void ModelsManager::updateMeshRawVertexPositions(std::vector<glm::vec3> vertexPositions, ModelIndex modelID, int meshIndex) {
        // Note: the input to this is assumed to be WELDED vertices WELDED via mesh.buildWeldMap. Need to GO BACK to unwelded

        Mesh* m = models[modelID].getMesh(meshIndex);
        int nVerticesModelMesh = m->vertexCount();
        int nVerticesInput = vertexPositions.size();
        
        if (nVerticesModelMesh != nVerticesInput) {
            std::cout << "Welded v. unwelded discrepancy, need to unweld\n";
            if (m->unweldedIndexToWeldedIndex_map.size() != m->vertexCount()) {
                std::cout << "BUG: m->unweldedIndexToWeldedIndex_map.size() != m->vertexCount(), are you using the correct mesh index? Did the welded mesh or your input vertex positions change in number of vertices? \n";
                return;
            }
            for (int i = 0; i < nVerticesModelMesh; i++) {
                m->vertices[i].position = vertexPositions[m->unweldedIndexToWeldedIndex_map[i]];
            }

        } else {
            std::cout << "updateMeshRawVertexPositions no need for welded remap\n";
            for (int i = 0; i < nVerticesModelMesh; i++) {
                m->vertices[i].position = vertexPositions[i];
            }
        }
    }
}