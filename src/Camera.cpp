#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/gtx/quaternion.hpp>
#include <glm/ext.hpp>
#include <iostream>

void Camera::update(float deltaTime) {
   this->deltaTime = deltaTime;

    // Build rotation quaternion
    glm::quat pitchRotation = glm::angleAxis(pitch, glm::vec3{-1.f, 0.f, 0.f});
    glm::quat yawRotation   = glm::angleAxis(yaw,   glm::vec3{0.f, -1.f, 0.f});
    glm::quat rotation      = yawRotation * pitchRotation;

    // Convert to matrix
    glm::mat4 rotMat = glm::toMat4(rotation);

    // Extract forward/right from rotation matrix
    glm::vec3 forward = glm::normalize(-glm::vec3(rotMat[2])); // camera looks down -Z
    glm::vec3 right   = glm::normalize(glm::vec3(rotMat[0]));
    glm::vec3 up      = glm::vec3(0, 1, 0);

    // Build movement direction using your old velocity mapping
    glm::vec3 moveDir =
        right   * velocity.x +   // A/D
        up      * velocity.z +   // space/shift
        forward * velocity.y;    // W/S

    position += moveDir * deltaTime;
   }

void Camera::processSDLInputEvent(SDL_Event* e) {
    if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN && e->button.button == SDL_BUTTON_LEFT) {
        isDragging = true;
    }
    if (e->type == SDL_EVENT_MOUSE_BUTTON_UP && e->button.button == SDL_BUTTON_LEFT) {
        isDragging = false;
    }

    if (e->type == SDL_EVENT_MOUSE_MOTION && isDragging) {
        float sensitivity = 0.1f;
        yaw   -= (float)e->motion.xrel * deltaTime * sensitivity;
        pitch += (float)e->motion.yrel * deltaTime * sensitivity;

        if (pitch >  89.0f) pitch =  89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }
}

glm::mat4 Camera::getViewTransform() {
    glm::quat pitchRotation = glm::angleAxis(pitch, glm::vec3{1.f, 0.f, 0.f});
    glm::quat yawRotation   = glm::angleAxis(yaw,   glm::vec3{0.f, -1.f, 0.f});
    glm::quat rotation      = yawRotation * pitchRotation;

    glm::mat4 rotMat = glm::toMat4(rotation);

    glm::vec3 forward = glm::normalize(-glm::vec3(rotMat[2]));
    glm::vec3 target  = position + forward;

    return glm::lookAt(position, target, glm::vec3(0,1,0));
}
void Camera::setYawPitch(float yaw, float pitch) {
    Camera::yaw = yaw;
    Camera::pitch = pitch;
}
 
glm::mat4 Camera::getRotationTransform() {
    glm::quat pitchRotation = glm::angleAxis(pitch, glm::vec3 { -1.f, 0.f, 0.f });
    glm::quat yawRotation = glm::angleAxis(yaw, glm::vec3 { 0.f, -1.f, 0.f });
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
