#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/gtx/quaternion.hpp>
#include <glm/ext.hpp>
#include <iostream>


void Camera::update() {
    float deltaTime = 0.01f;
    glm::mat4 cameraRotation = getRotationTransform();
    std::cout << cameraRotation[0][0] << std::endl;
    position += deltaTime * glm::vec3(cameraRotation * glm::vec4(velocity, 0.f));
   }

void Camera::processSDLInputEvent(SDL_Event* e) {
    float usingManhattanSpeed = 10.;
    if (e->type == SDL_EVENT_KEY_DOWN) {
        std::cout << e->key.scancode << std::endl; 
        if (e->key.scancode == SDL_SCANCODE_W) { velocity.z = -usingManhattanSpeed; }
        if (e->key.scancode == SDL_SCANCODE_S) { velocity.z = usingManhattanSpeed; }
        if (e->key.scancode == SDL_SCANCODE_A) { velocity.x = -usingManhattanSpeed; }
        if (e->key.scancode == SDL_SCANCODE_D) { velocity.x = usingManhattanSpeed; }
    }

    if (e->type == SDL_EVENT_KEY_UP) {
        if (e->key.scancode == SDL_SCANCODE_W) { velocity.z = 0; }
        if (e->key.scancode == SDL_SCANCODE_S) { velocity.z = 0; }
        if (e->key.scancode == SDL_SCANCODE_A) { velocity.x = 0; }
        if (e->key.scancode == SDL_SCANCODE_D) { velocity.x = 0; }
    }

    if (e->type == SDL_EVENT_MOUSE_MOTION) {
        yaw -= (float)e->motion.xrel / 100.f;
        pitch += (float)e->motion.yrel / 100.f;
    }
}

glm::mat4 Camera::getViewTransform() {
    glm::mat4 cameraRotation = getRotationTransform();
    glm::mat4 cameraTranslation = glm::translate(glm::mat4(1.f), -position);
    return cameraRotation * cameraTranslation;
}
void Camera::setYawPitch(float yaw, float pitch) {
    Camera::yaw = yaw;
    Camera::pitch = pitch;
}
 
glm::mat4 Camera::getRotationTransform() {
    glm::quat pitchRotation = glm::angleAxis(pitch, glm::vec3 { 1.f, 0.f, 0.f });
    glm::quat yawRotation = glm::angleAxis(yaw, glm::vec3 { 0.f, 1.f, 0.f });
    return glm::toMat4(yawRotation) * glm::toMat4(pitchRotation);
}

// Produces a matrix that, when multiplied by a point (x, y, z, 1) that is within the view volume,
// will result in a point (x', y', z', w') in clip space s.t. -w' <= {x',y',z'} <= w'. If not
// in the view volume, the triangle will later need to be clipped
glm::mat4 Camera::perspectiveTransform(float fovRadians, float aspect, float nearPlane, float farPlane) {
    float f = 1.0f / glm::tan(fovRadians * 0.5f);
    glm::mat4 m(0.0f);
    m[0][0] = f / aspect;
    m[1][1] = f;
    m[2][2] = (farPlane + nearPlane) / (nearPlane - farPlane);
    m[2][3] = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);
    m[3][2] = -1.0f;
    return m;
}
