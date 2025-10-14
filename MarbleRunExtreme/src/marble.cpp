#include "marble.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>
#include <iostream>

// Generate a simple UV sphere
static void createSphere(std::vector<float>& vertices, std::vector<unsigned int>& indices, int sectorCount = 36, int stackCount = 18) {
    float x, y, z, xy;
    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i) {
        stackAngle = M_PI / 2 - i * stackStep;
        xy = cosf(stackAngle);
        z = sinf(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;
            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);
            vertices.insert(vertices.end(), { x, y, z });
        }
    }

    for (int i = 0; i < stackCount; ++i) {
        int k1 = i * (sectorCount + 1);
        int k2 = k1 + sectorCount + 1;
        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(static_cast<unsigned int>(k1));
                indices.push_back(static_cast<unsigned int>(k2));
                indices.push_back(static_cast<unsigned int>(k1 + 1));
            }
            if (i != (stackCount - 1)) {
                indices.push_back(static_cast<unsigned int>(k1 + 1));
                indices.push_back(static_cast<unsigned int>(k2));
                indices.push_back(static_cast<unsigned int>(k2 + 1));
            }
        }
    }
}

Marble::Marble(glm::vec3 pos, glm::vec3 col, float r)
    : position(pos), color(col), radius(r) {}

void Marble::draw(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection)
{
    static GLuint VAO = 0, VBO = 0, EBO = 0;
    static std::vector<unsigned int> indices;

    if (VAO == 0) {
        std::vector<float> vertices;
        createSphere(vertices, indices);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    glUseProgram(shaderProgram);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, glm::vec3(radius));
    glm::mat4 transform = projection * view * model;

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(transform));
    glUniform3fv(glGetUniformLocation(shaderProgram, "marbleColor"), 1, glm::value_ptr(color));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
}
