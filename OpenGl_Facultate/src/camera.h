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
    // Movement speed
    float MovementSpeed;

    // Constructor with vectors
    Camera(glm::vec3 position, glm::vec3 up);

    // Returns the view matrix calculated using LookAt
    glm::mat4 GetViewMatrix();

    // Processes keyboard input
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

private:
    // Updates the front, right, and up vectors from the current values
    void updateCameraVectors();
};

#endif
