#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class Skybox{
public:
    Skybox(const std::vector<std::string>& faces);
    Skybox(const std::string& atlasPath);
    ~Skybox();
    
    void draw(const glm::mat4& view, const glm::mat4& projection, GLuint shaderProgram);
    
private:
    GLuint cubemapTexture;
    GLuint VAO, VBO;

    GLuint loadCubemap(const std::vector<std::string>& faces);
    GLuint loadCubemapFromAtlas(const std::string& atlasPath);
};
