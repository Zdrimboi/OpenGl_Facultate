#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, float deltaTime);

// Global camera instance, starting at (0,0,5)
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f));

int main()
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Colored Cube with Camera", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load OpenGL function pointers with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Build and compile our shader program from external files.
    // Make sure "cube.vs" and "cube.fs" are in your working directory.
    Shader ourShader("assets/skybox.vs", "assets/skybox.fs");

    // Define the vertices for a cube with a unique solid color for each face.
    // Each face is drawn as two triangles (6 vertices per face).
    float vertices[] = {
        // Positions           // Colors
        // Front face (red)a
        -1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 0.0f,
         1.0f,  1.0f,  1.0f,   1.0f, 0.0f, 0.0f,
         1.0f,  1.0f,  1.0f,   1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,   1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 0.0f,

        // Back face (green)
        -1.0f, -1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,   0.0f, 1.0f, 0.0f,

        // Left face (blue)
        -1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,   0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,   0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,   0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f,

        // Right face (yellow)
         1.0f,  1.0f,  1.0f,   1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,   1.0f, 1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 0.0f,
         1.0f, -1.0f,  1.0f,   1.0f, 1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,   1.0f, 1.0f, 0.0f,

         // Top face (cyan)
         -1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 1.0f,
          1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 1.0f,
          1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 1.0f,
          1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 1.0f,
         -1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 1.0f,
         -1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 1.0f,

         // Bottom face (magenta)
         -1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 1.0f,
          1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 1.0f,
          1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 1.0f,
          1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 1.0f,
         -1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 1.0f,
         -1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 1.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Timing variables for movement
    float lastFrame = 0.0f, deltaTime = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        // Create transformation matrices:
        glm::mat4 model = glm::mat4(1.0f);
        // Rotate the cube over time so you can see multiple faces
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.f / 600.f, 0.1f, 100.0f);

        ourShader.setMat4("model", model);
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}
