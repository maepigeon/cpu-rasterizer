#include "Mesh.hpp"
#include <cmath>
#include <map>
#include <iostream>

int Mesh::vertexCount() {
    return vertices.size();
}

int Mesh::indexCount() {
    return indices.size();
}

void Mesh::buildWeldMap(float tolerance = 1e-5) {
    weldedPositions.clear();
    unweldedIndexToWeldedIndex_map.assign(vertices.size(), 0);
    std::map<std::array<int64_t, 3>, uint32_t> visitedPositions;
    for (int i = 0; i < vertices.size(); i++) {
        int64_t weldToGridX = (int64_t)llround(vertices[i].position.x / tolerance);
        int64_t weldToGridY = (int64_t)llround(vertices[i].position.y / tolerance);
        int64_t weldToGridZ = (int64_t)llround(vertices[i].position.z / tolerance);
        std::array<int64_t, 3> key = {weldToGridX, weldToGridY, weldToGridZ};
        std::map<std::array<int64_t, 3>, uint32_t>::iterator found = visitedPositions.find(key);
        if (found != visitedPositions.end()) {
            unweldedIndexToWeldedIndex_map[i] = found->second;
        } else {
            weldedPositions.push_back(vertices[i].position);
            visitedPositions[key] = weldedPositions.size() - 1;
            unweldedIndexToWeldedIndex_map[i] = weldedPositions.size() - 1;
        }
    }
}

void Mesh::initializeFromWelded(std::vector<glm::vec3>& positions, std::vector<uint32_t>& weldedIndices) {
    weldedPositions = positions;
    indices = weldedIndices;
    vertices.resize(positions.size());
    unweldedIndexToWeldedIndex_map.resize(positions.size());
    for (uint32_t i = 0; i < positions.size(); i++) {
        vertices[i].position = positions[i];
        vertices[i].normal = glm::vec3(0.f);
        vertices[i].texcoord = glm::vec2(0.f);
        unweldedIndexToWeldedIndex_map[i] = i;
    }
}

float Mesh::calculateTolerance() {
    if (vertices.empty()) {
        return 1e-5f;
    }
    glm::vec3 minBounds = vertices[0].position;
    glm::vec3 maxBounds = vertices[0].position;
    for (int i = 1; i < vertices.size(); i++) {
        minBounds = glm::min(minBounds, vertices[i].position);
        maxBounds = glm::max(maxBounds, vertices[i].position);
    }
    float distance = glm::length(maxBounds - minBounds);
    if (distance < 1e-12f) {
        return 1e-5f; // all vertices coincident
    }
    return distance * 1e-6f;
}

void Mesh::calculateSmoothNormalsFromWelded() {
    if (unweldedIndexToWeldedIndex_map.size() != vertices.size()) {
        std::cout << "Provided unweldedIndexToWeldedIndex_map does not match vertices size, rebuilding..." << std::endl;
        buildWeldMap();
    }
    std::vector<glm::vec3> weldedNormals(weldedPositions.size(), glm::vec3(0.f));
    for (int i = 0; i + 2 < indices.size(); i += 3) {
        uint32_t i0 = indices[i];
        uint32_t i1 = indices[i + 1];
        uint32_t i2 = indices[i + 2];
        glm::vec3 v0 = vertices[i0].position;
        glm::vec3 v1 = vertices[i1].position;
        glm::vec3 v2 = vertices[i2].position;
        glm::vec3 faceNormal = glm::cross(v1 - v0, v2 - v0);
        weldedNormals[unweldedIndexToWeldedIndex_map[i0]] += faceNormal;
        weldedNormals[unweldedIndexToWeldedIndex_map[i1]] += faceNormal;
        weldedNormals[unweldedIndexToWeldedIndex_map[i2]] += faceNormal;
    }
    for (int i = 0; i < weldedNormals.size(); i++) {
        float normalLength = glm::length(weldedNormals[i]);
        if (normalLength > 1e-8f) {
            weldedNormals[i] /= normalLength;
        } else {
            weldedNormals[i] = glm::vec3(0.f, 0.f, 1.f);
        }
    }
    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].normal = weldedNormals[unweldedIndexToWeldedIndex_map[i]];
    }
}


void Mesh::calculateFlatNormals() {
    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].normal = glm::vec3(0.f);
    }
    for (int i = 0; i + 2 < indices.size(); i += 3) {
        uint32_t i0 = indices[i];
        uint32_t i1 = indices[i + 1];
        uint32_t i2 = indices[i + 2];
        glm::vec3 v0 = vertices[i0].position;
        glm::vec3 v1 = vertices[i1].position;
        glm::vec3 v2 = vertices[i2].position;
        glm::vec3 faceNormal = glm::cross(v1 - v0, v2 - v0);
        vertices[i0].normal += faceNormal;
        vertices[i1].normal += faceNormal;
        vertices[i2].normal += faceNormal;
    }
    for (int i = 0; i < vertices.size(); i++) {
        float normalLength = glm::length(vertices[i].normal);
        if (normalLength > 1e-8) {
            vertices[i].normal = glm::normalize(vertices[i].normal);
        } else {
            vertices[i].normal = {0.f, 0.f, 1.f};
        }
    }
}