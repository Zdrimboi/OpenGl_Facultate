#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Callback to adjust viewport when window resizes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Process input (close window on ESC)
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "Colored Cube Test", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD before calling any OpenGL function
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Define cube vertices with positions and a solid color per face
    // Each face is defined by 6 vertices (2 triangles) and is assigned a unique color.
    float vertices[] = {
        // Positions          // Colors
        // Front face (red)
        -1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 0.0f, // bottom left
         1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 0.0f, // bottom right
         1.0f,  1.0f,  1.0f,   1.0f, 0.0f, 0.0f, // top right

         1.0f,  1.0f,  1.0f,   1.0f, 0.0f, 0.0f, // top right
        -1.0f,  1.0f,  1.0f,   1.0f, 0.0f, 0.0f, // top left
        -1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 0.0f, // bottom left

        // Back face (green)
        -1.0f, -1.0f, -1.0f,   0.0f, 1.0f, 0.0f, // bottom left
         1.0f, -1.0f, -1.0f,   0.0f, 1.0f, 0.0f, // bottom right
         1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f, // top right

         1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f, // top right
        -1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f, // top left
        -1.0f, -1.0f, -1.0f,   0.0f, 1.0f, 0.0f, // bottom left

        // Left face (blue)
        -1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f, // top right
        -1.0f,  1.0f, -1.0f,   0.0f, 0.0f, 1.0f, // top left
        -1.0f, -1.0f, -1.0f,   0.0f, 0.0f, 1.0f, // bottom left

        -1.0f, -1.0f, -1.0f,   0.0f, 0.0f, 1.0f, // bottom left
        -1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 1.0f, // bottom right
        -1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f, // top right

        // Right face (yellow)
         1.0f,  1.0f,  1.0f,   1.0f, 1.0f, 0.0f, // top left
         1.0f,  1.0f, -1.0f,   1.0f, 1.0f, 0.0f, // top right
         1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 0.0f, // bottom right

         1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 0.0f, // bottom right
         1.0f, -1.0f,  1.0f,   1.0f, 1.0f, 0.0f, // bottom left
         1.0f,  1.0f,  1.0f,   1.0f, 1.0f, 0.0f, // top left

         // Top face (cyan)
         -1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 1.0f, // bottom left
          1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 1.0f, // bottom right
          1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 1.0f, // top right

          1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 1.0f, // top right
         -1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 1.0f, // top left
         -1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 1.0f, // bottom left

         // Bottom face (magenta)
         -1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 1.0f, // top left
          1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 1.0f, // top right
          1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 1.0f, // bottom right

          1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 1.0f, // bottom right
         -1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 1.0f, // bottom left
         -1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 1.0f  // top left
    };

    // Generate and bind VAO and VBO
    unsigned int VAO, VBO;
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

    // Define simple vertex and fragment shaders
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "out vec3 ourColor;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "   ourColor = aColor;\n"
        "}\0";

    const char* fragmentShaderSource = "#version 330 core\n"
        "in vec3 ourColor;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(ourColor, 1.0);\n"
        "}\n\0";

    // Compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // (Check for compile errors here if needed)

    // Compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // (Check for compile errors here if needed)

    // Link shaders into a shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // (Check for linking errors here if needed)

    // Delete the shader objects once linked
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Set up transformation matrices
    glm::mat4 model = glm::mat4(1.0f);
    // Move the cube slightly away from the camera
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.f / 600.f, 0.1f, 100.0f);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Pass transformation matrices to shader
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

        // In the render loop, before drawing:
        glm::mat4 model = glm::mat4(1.0f);
        // Rotate the cube over time so you can see multiple faces
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);



        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}
