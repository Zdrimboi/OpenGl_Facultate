#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "camera.h"
#include "Model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <stb/stb_image.h>

// ─────────────────── callbacks ───────────────────
void framebuffer_size_callback(GLFWwindow* window, int w, int h) { glViewport(0, 0, w, h); }
void processInput(GLFWwindow* win, float dt);
void mouse_button_callback(GLFWwindow* w, int b, int a, int m);
void mouse_callback(GLFWwindow* w, double x, double y);
unsigned int loadCubemap(const std::vector<std::string>& faces);

// ─────────────────── globals ─────────────────────
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
bool   rightMouse = false, firstMouse = true;
float  lastX = 400.f, lastY = 300.f;

// ─────────────────── main ────────────────────────
int main()
{
    // GLFW/GLAD -------------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(800, 600, "Model + Plane + Sun + Skybox", nullptr, nullptr);
    if (!window) { std::cerr << "Failed to create window\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD\n"; return -1;
    }
    glEnable(GL_DEPTH_TEST);

    // ───── shaders ────────────────────────────────────────────
    Shader litShader("assets/dirLight.vs", "assets/dirLight.fs");   // NEW
    Shader skyShader("assets/skybox.vs", "assets/skybox.fs");

    // ───── model (tree) ───────────────────────────────────────
    Model tree("C:/Users/alexx/Downloads/tree/tree1_3ds/Tree1.3ds");   // adjust path

    // ───── ground plane geometry ──────────────────────────────
    float plane[] = {
        // pos                  normal        tex
         25,-1, 25,  0,1,0,   25,0,
        -25,-1, 25,  0,1,0,    0,0,
        -25,-1,-25,  0,1,0,    0,25,
         25,-1, 25,  0,1,0,   25,0,
        -25,-1,-25,  0,1,0,    0,25,
         25,-1,-25,  0,1,0,   25,25
    };
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // ───── skybox geometry (unchanged) ────────────────────────
    float skyV[] = { // 8 verts
       -1,-1,-1, 1,-1,-1, 1,1,-1, -1,1,-1,
       -1,-1, 1, 1,-1, 1, 1,1, 1, -1,1, 1 };
    unsigned skyI[] = {
        0,1,2, 2,3,0, 4,5,6, 6,7,4, 0,3,7, 7,4,0,
        1,5,6, 6,2,1, 3,2,6, 6,7,3, 0,1,5, 5,4,0 };
    unsigned skyVAO, skyVBO, skyEBO;
    glGenVertexArrays(1, &skyVAO); glGenBuffers(1, &skyVBO); glGenBuffers(1, &skyEBO);
    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyV), skyV, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyI), skyI, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // ───── skybox texture ─────────────────────────────────────
    std::vector<std::string> faces = {
        "assets/Textures/right.jpg","assets/Textures/left.jpg",
        "assets/Textures/top.jpg"  ,"assets/Textures/bottom.jpg",
        "assets/Textures/front.jpg","assets/Textures/back.jpg" };
    unsigned int cubemap = loadCubemap(faces);
    skyShader.use(); skyShader.setInt("skybox", 0);

    // ───── sunlight (dir-light) uniforms (static) ─────────────
    litShader.use();
    litShader.setVec3("light.direction", glm::vec3(-0.3f, -1.0f, -0.2f));
    litShader.setVec3("light.ambient", glm::vec3(0.25f));
    litShader.setVec3("light.diffuse", glm::vec3(0.9f));
    litShader.setVec3("light.specular", glm::vec3(1.0f));
    litShader.setFloat("shininess", 32.0f);

    // ───── main loop ──────────────────────────────────────────
    float last = 0.f;
    while (!glfwWindowShouldClose(window))
    {
        float now = (float)glfwGetTime(), dt = now - last; last = now;
        processInput(window, dt);

        glClearColor(0.1f, 0.1f, 0.15f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 proj = glm::perspective(glm::radians(45.f), 800.f / 600.f, 0.1f, 100.f);

        // common lit uniforms (view/proj/eye)
        litShader.use();
        litShader.setMat4("view", view);
        litShader.setMat4("projection", proj);
        litShader.setVec3("viewPos", camera.Position);

        // ---- 1. tree model ------------------------------------------------
        glm::mat4 model = glm::mat4(1.0f);
        litShader.setMat4("model", model);
        litShader.setBool("useTexture", true);
        tree.Draw(litShader);

        // ---- 2. ground plane ---------------------------------------------
        litShader.setBool("useTexture", false);
        litShader.setVec3("objectColor", glm::vec3(0.35f, 0.55f, 0.25f));
        litShader.setMat4("model", glm::mat4(1.0f));
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // ---- 3. skybox ----------------------------------------------------
        glDepthFunc(GL_LEQUAL);
        skyShader.use();
        skyShader.setMat4("view", glm::mat4(glm::mat3(view)));
        skyShader.setMat4("projection", proj);
        glBindVertexArray(skyVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        glfwSwapBuffers(window); glfwPollEvents();
    }

    // ───── cleanup ────────────────────────────────────────────
    glDeleteVertexArrays(1, &planeVAO); glDeleteBuffers(1, &planeVBO);
    glDeleteVertexArrays(1, &skyVAO);   glDeleteBuffers(1, &skyVBO); glDeleteBuffers(1, &skyEBO);
    glfwTerminate();
    return 0;
}

// ───────────────────────── helpers ───────────────────────────
void processInput(GLFWwindow* w, float dt)
{
    if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(w, true);
    if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, dt);
    if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, dt);
    if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, dt);
    if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, dt);
}
void mouse_button_callback(GLFWwindow*, int button, int action, int)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) { rightMouse = true; firstMouse = true; }
        else if (action == GLFW_RELEASE) { rightMouse = false; }
    }
}
void mouse_callback(GLFWwindow*, double xpos, double ypos)
{
    if (!rightMouse) return;
    if (firstMouse) { lastX = (float)xpos; lastY = (float)ypos; firstMouse = false; }
    float xoff = (float)xpos - lastX, yoff = lastY - (float)ypos;
    lastX = (float)xpos; lastY = (float)ypos;
    camera.ProcessMouseMovement(xoff, yoff);
}

unsigned int loadCubemap(const std::vector<std::string>& faces)
{
    stbi_set_flip_vertically_on_load(false); glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    unsigned int tex; glGenTextures(1, &tex); glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
    int w, h, n; for (unsigned i = 0; i < faces.size(); ++i) {
        unsigned char* data = stbi_load(faces[i].c_str(), &w, &h, &n, 0);
        if (data) {
            GLenum fmt = (n == 4 ? GL_RGBA : GL_RGB);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else { std::cerr << "Cubemap failed at " << faces[i] << "\n"; }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return tex;
}
