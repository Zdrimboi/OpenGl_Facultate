#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Define camera movement directions
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera {
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler Angles
    float Yaw;
    float Pitch;
    // Options
    float MovementSpeed;
    float MouseSensitivity;

    // Constructor with vectors
    Camera(glm::vec3 position, glm::vec3 up);

    // Returns the view matrix calculated using LookAt
    glm::mat4 GetViewMatrix();

    // Processes keyboard input
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // Processes mouse movement input when right button is held
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

private:
    // Updates the camera vectors based on updated Euler angles
    void updateCameraVectors();
};

#endif
