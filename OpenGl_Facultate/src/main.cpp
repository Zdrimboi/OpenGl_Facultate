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

// ── callbacks ──────────────────────────────────────────────────────────
void framebuffer_size_callback(GLFWwindow*, int, int);
void processInput(GLFWwindow*, float);
void mouse_button_callback(GLFWwindow*, int, int, int);
void mouse_callback(GLFWwindow*, double, double);
unsigned int loadCubemap(const std::vector<std::string>&);

// ── globals ────────────────────────────────────────────────────────────
Camera camera(glm::vec3(0, 0, 5), glm::vec3(0, 1, 0));
bool   RMB = false, firstMouse = true;
float  lastX = 400, lastY = 300;

// ── constants for the shadow map ───────────────────────────────────────
const unsigned SHADOW_W = 4096, SHADOW_H = 4096;

int main()
{
    // GLFW / GLAD --------------------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* win = glfwCreateWindow(800, 600, "Shadows + Skybox", nullptr, nullptr);
    if (!win) { std::cerr << "Window creation failed\n"; return -1; }
    glfwMakeContextCurrent(win);
    glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);
    glfwSetMouseButtonCallback(win, mouse_button_callback);
    glfwSetCursorPosCallback(win, mouse_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD init failed\n"; return -1;
    }
    glEnable(GL_DEPTH_TEST);

    // ── shaders ────────────────────────────────────────────────────────
    Shader litShader("assets/dirShadow.vs", "assets/dirShadow.fs"); // lighting + shadows
    Shader depthShader("assets/depth.vs", "assets/depth.fs");     // depth-only
    Shader skyShader("assets/skybox.vs", "assets/skybox.fs");

    // ── model (tree) ----------------------------------------------------
    Model tree("C:/Users/alexx/Downloads/tree/tree1_3ds/Tree1.3ds");

    // ── ground plane ----------------------------------------------------
    float plane[] = {
        // pos                normal  tex
        25,-1, 25, 0,1,0, 25,0,
       -25,-1, 25, 0,1,0,  0,0,
       -25,-1,-25, 0,1,0,  0,25,
        25,-1, 25, 0,1,0, 25,0,
       -25,-1,-25, 0,1,0,  0,25,
        25,-1,-25, 0,1,0, 25,25
    };
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO); glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);  glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // ── skybox geometry -------------------------------------------------
    float skyV[] = { -1,-1,-1, 1,-1,-1, 1,1,-1,-1,1,-1,-1,-1,1, 1,-1,1, 1,1,1,-1,1,1 };
    unsigned skyI[] = { 0,1,2,2,3,0, 4,5,6,6,7,4, 0,3,7,7,4,0,
                     1,5,6,6,2,1, 3,2,6,6,7,3, 0,1,5,5,4,0 };
    unsigned skyVAO, skyVBO, skyEBO;
    glGenVertexArrays(1, &skyVAO); glGenBuffers(1, &skyVBO); glGenBuffers(1, &skyEBO);
    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyV), skyV, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyI), skyI, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // ── cubemap texture -------------------------------------------------
    std::vector<std::string> faces = {
        "assets/Textures/right.jpg","assets/Textures/left.jpg",
        "assets/Textures/top.jpg"  ,"assets/Textures/bottom.jpg",
        "assets/Textures/front.jpg","assets/Textures/back.jpg" };
    unsigned int cubemap = loadCubemap(faces);
    skyShader.use(); skyShader.setInt("skybox", 0);

    // ── shadow map FBO --------------------------------------------------
    unsigned int depthFBO;
    glGenFramebuffers(1, &depthFBO);

    unsigned int depthTex;
    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_W, SHADOW_H, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float border[4] = { 1,1,1,1 }; glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
    glDrawBuffer(GL_NONE); glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ── static light parameters ----------------------------------------
    glm::vec3 lightDir(-0.3f, -1.0f, -0.2f);
    litShader.use();
    litShader.setVec3("light.direction", lightDir);
    litShader.setVec3("light.ambient", glm::vec3(0.25f));
    litShader.setVec3("light.diffuse", glm::vec3(0.9f));
    litShader.setVec3("light.specular", glm::vec3(1.0f));
    litShader.setFloat("shininess", 32.0f);
    litShader.setInt("shadowMap", 1);           // depthTex bound to unit 1
    litShader.setMat4("lightSpaceMatrix", glm::mat4(1)); // placeholder

    // ── render loop -----------------------------------------------------
    float last = 0;
    float sunSpeed = 0.05f;        // revolutions per second  (change to taste)
    while (!glfwWindowShouldClose(win))
    {
        float now = (float)glfwGetTime(), dt = now - last; last = now;
        processInput(win, dt);

        // 1. create light-space matrix (orthographic)
        const float nearP = 1.f, farP = 50.f, ortho = 20.f;

        /*  ─── change this BEGIN ──────────────────────────────── */
        float angle = now * glm::two_pi<float>() * sunSpeed;     // 0-360° over time
        glm::vec3 lightDir = glm::normalize(
            glm::vec3(cos(angle),          // X   <-- rotates horizontally
                -0.8f,               // Y   (keep negative so light points down)
                sin(angle)));        // Z
        /*  ─── change this END   ──────────────────────────────── */

        glm::mat4 lightProj = glm::ortho(-ortho, ortho, -ortho, ortho, nearP, farP);
        glm::mat4 lightView = glm::lookAt(-lightDir * 20.f, glm::vec3(0), glm::vec3(0, 1, 0));
        glm::mat4 lightSpace = lightProj * lightView;


        // 2. render depth map -------------------------------------------
        glViewport(0, 0, SHADOW_W, SHADOW_H);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        depthShader.use();
        depthShader.setMat4("lightSpaceMatrix", lightSpace);

        //   2a. tree (rotated 90° Y)
        glm::mat4 model = glm::rotate(glm::mat4(1), glm::radians(-90.f), glm::vec3(1, 0, 0));
        depthShader.setMat4("model", model);
        tree.Draw(depthShader);

        //   2b. plane
        depthShader.setMat4("model", glm::mat4(1));
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 3. normal render pass -----------------------------------------
        int width, height; glfwGetFramebufferSize(win, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.15f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 proj = glm::perspective(glm::radians(45.f), (float)width / height, 0.1f, 100.f);

        //   3a. lit objects (plane + tree)
        litShader.use();
        litShader.setVec3("light.direction", lightDir);
        litShader.setMat4("view", view);
        litShader.setMat4("projection", proj);
        litShader.setVec3("viewPos", camera.Position);
        litShader.setMat4("lightSpaceMatrix", lightSpace);
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, depthTex);

        // tree
        litShader.setBool("useTexture", true);
        model = glm::rotate(glm::mat4(1), glm::radians(-90.f), glm::vec3(1, 0, 0));
        litShader.setMat4("model", model);
        tree.Draw(litShader);

        // plane
        litShader.setBool("useTexture", false);
        litShader.setVec3("objectColor", glm::vec3(0.35f, 0.55f, 0.25f));
        litShader.setMat4("model", glm::mat4(1));
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        //   3b. skybox
        glDepthFunc(GL_LEQUAL);
        skyShader.use();
        skyShader.setMat4("view", glm::mat4(glm::mat3(view)));
        skyShader.setMat4("projection", proj);
        glBindVertexArray(skyVAO);
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        glfwSwapBuffers(win); glfwPollEvents();
    }

    // ── cleanup ---------------------------------------------------------
    glDeleteVertexArrays(1, &planeVAO); glDeleteBuffers(1, &planeVBO);
    glDeleteVertexArrays(1, &skyVAO);   glDeleteBuffers(1, &skyVBO); glDeleteBuffers(1, &skyEBO);
    glDeleteFramebuffers(1, &depthFBO); glDeleteTextures(1, &depthTex);
    glfwTerminate();
    return 0;
}

// ── helper implementations ─────────────────────────────────────────────
void framebuffer_size_callback(GLFWwindow*, int w, int h) { glViewport(0, 0, w, h); }
void processInput(GLFWwindow* w, float dt) {
    if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(w, true);
    if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, dt);
    if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, dt);
    if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, dt);
    if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, dt);
}
void mouse_button_callback(GLFWwindow*, int b, int a, int) {
    if (b == GLFW_MOUSE_BUTTON_RIGHT) {
        if (a == GLFW_PRESS) { RMB = true; firstMouse = true; }
        else if (a == GLFW_RELEASE) { RMB = false; }
    }
}
void mouse_callback(GLFWwindow*, double xpos, double ypos) {
    if (!RMB) return;
    if (firstMouse) { lastX = (float)xpos; lastY = (float)ypos; firstMouse = false; }
    float xoff = (float)xpos - lastX, yoff = lastY - (float)ypos;
    lastX = (float)xpos; lastY = (float)ypos;
    camera.ProcessMouseMovement(xoff, yoff);
}

unsigned int loadCubemap(const std::vector<std::string>& faces)
{
    stbi_set_flip_vertically_on_load(false); glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    unsigned int tex; glGenTextures(1, &tex); glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
    int w, h, n;
    for (unsigned i = 0; i < faces.size(); ++i) {
        unsigned char* data = stbi_load(faces[i].c_str(), &w, &h, &n, 0);
        if (data) {
            GLenum fmt = (n == 4 ? GL_RGBA : GL_RGB);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else { std::cerr << "Failed cubemap " << faces[i] << "\n"; }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return tex;
}
