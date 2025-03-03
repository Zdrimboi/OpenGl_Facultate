#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

class Shader {
public:
    // The program ID
    unsigned int ID;

    // Constructor reads and builds the shader from file paths
    Shader(const char* vertexPath, const char* fragmentPath);

    // Activate the shader
    void use();

    // Utility functions to set uniforms
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
};

#endif
