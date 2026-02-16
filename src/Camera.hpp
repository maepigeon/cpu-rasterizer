#ifndef CAMERA
#define CAMERA

#include <SDL3/SDL_events.h>
#include <glm/glm.hpp>
glm::mat4 createOrthographicPerspectiveTransform(float left, float right, float bottom, float top, float near, float far);
glm::mat4 perspectiveTransform(float fovRadians, float aspect, float nearPlane, float farPlane);

class Camera {
public:
    glm::vec3 position { 0.f, 0.f, 0.f };
    glm::vec3 velocity { 0.f, 0.f, 0.f };
    glm::mat4 getViewTransform();
    glm::mat4 getRotationTransform();
    glm::mat4 perspectiveTransform(float fovRadians, float aspect, float nearPlane, float farPlane);
    void setYawPitch(float yaw, float pitch);
    void processSDLInputEvent(SDL_Event* e);
    void update();
private:
    // vertical rotation
    float pitch { 0.f };
    // horizontal rotation
    float yaw { 0.f };
};
#endif