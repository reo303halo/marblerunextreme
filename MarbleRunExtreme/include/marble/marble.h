#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>

class Marble {
public:
    glm::vec3 position;
    glm::vec3 color;
    float radius;

    Marble(glm::vec3 pos, glm::vec3 col, float r);

    void draw(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection);
};
