#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <stb/stb_image.h>  // Make sure to have stb_image in your include path

// Callback declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, float deltaTime);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// Function to load a cubemap texture from 6 individual texture faces
unsigned int loadCubemap(const std::vector<std::string>& faces);

// Global camera instance, starting at (0,0,5)
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f));

// Mouse state globals for free-look when holding right mouse button
bool rightMousePressed = false;
float lastX = 400, lastY = 300;
bool firstMouse = true;

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
    GLFWwindow* window = glfwCreateWindow(800, 600, "Cube & Skybox Scene", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Load OpenGL function pointers with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    // Build and compile our shaders
    Shader cubeShader("assets/cube.vs", "assets/cube.fs");
    Shader skyboxShader("assets/skybox.vs", "assets/skybox.fs");

    // ----- Cube Setup -----
    // Define vertices for a colored cube (each face has a unique color)
    float cubeVertices[] = {
        // Positions           // Colors
        // Front face (red)
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

    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // ----- Skybox Setup -----
    // Skybox vertices (a cube)
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // Load cubemap texture for the skybox
    std::vector<std::string> faces = {
        "assets/Textures/right.jpg",
        "assets/Textures/left.jpg",
        "assets/Textures/top.jpg",
        "assets/Textures/bottom.jpg",
        "assets/Textures/front.jpg",
        "assets/Textures/back.jpg"
    };
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    unsigned int cubemapTexture = loadCubemap(faces);
    // Set skybox shader uniform
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // Timing variables for movement
    float lastFrame = 0.0f, deltaTime = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ----- Draw the Cube -----
        cubeShader.use();
        // Rotate the cube over time
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, currentFrame, glm::vec3(0.5f, 1.0f, 0.0f));
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.f / 600.f, 0.1f, 100.0f);
        cubeShader.setMat4("model", model);
        cubeShader.setMat4("view", view);
        cubeShader.setMat4("projection", projection);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // ----- Draw the Skybox -----
        // Change depth function so skybox is drawn behind all objects
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        // Remove translation from the view matrix
        glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));
        skyboxShader.setMat4("view", viewNoTranslation);
        skyboxShader.setMat4("projection", projection);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup resources
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glfwTerminate();
    return 0;
}

unsigned int loadCubemap(const std::vector<std::string>& faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            // Note: adjust the format based on your image (GL_RGB or GL_RGBA)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return textureID;
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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            rightMousePressed = true;
            firstMouse = true; // Reset to prevent large jumps on initial click
        }
        else if (action == GLFW_RELEASE)
        {
            rightMousePressed = false;
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!rightMousePressed)
        return;

    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos; // Reversed since y-coordinates go from bottom to top
    lastX = (float)xpos;
    lastY = (float)ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}
