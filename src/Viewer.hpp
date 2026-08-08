#ifndef VIEWER
#define VIEWER
#include <vector.h>
#include <glm/glm.hpp>

// Opens a window and renders the specified mesh
void displayMesh(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> uvs, std::vector<uint32_t> indices, int width, int height, float scaleWindow);

#endif