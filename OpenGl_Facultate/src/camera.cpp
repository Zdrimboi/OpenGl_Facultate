#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up)
    : Position(position), WorldUp(up), Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(2.5f)
{
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}
