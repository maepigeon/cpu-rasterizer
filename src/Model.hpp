#ifndef MODEL
#define MODEL

#include <vector>
#include "Mesh.hpp"
class Model {
public:
    std::vector<Mesh>& getMeshes();
    void addMesh(Mesh& mesh);
    bool empty();
    glm::mat4x4 worldTransform = glm::mat4x4(1.,0.,0.,0.,
                                             0.,1.,0.,0.,
                                             0.,0.,1.,0.,
                                             0.,0.,0.,1.);
private:
    std::vector<Mesh> meshes;
};
#endif