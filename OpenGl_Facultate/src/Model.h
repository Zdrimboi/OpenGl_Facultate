#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include "Mesh.h"
#include "shader.h"
#include <string>
#include <vector>

class Model {
public:
    // Model data
    std::vector<Texture> textures_loaded; // stores all the textures loaded so far
    std::vector<Mesh> meshes;
    std::string directory;

    // Constructor, expects a filepath to a 3D model.
    Model(const std::string& path);

    // Draw the model (and thus all its meshes)
    void Draw(Shader& shader);
private:
    // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(const std::string& path);

    // Processes a node in a recursive fashion.
    void processNode(aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    // Checks all material textures of a given type and loads the textures if they're not loaded yet.
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif
