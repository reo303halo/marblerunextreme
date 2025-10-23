#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class RenderableBox {
public:
    glm::vec3 size;    // half extents
    GLuint VAO, VBO;

    RenderableBox(const glm::vec3& halfExtents) : size(halfExtents) {
        float vertices[] = {
            // positions for each triangle (36 vertices)
            -1,-1,-1,  1,-1,-1,  1,1,-1,  1,1,-1, -1,1,-1, -1,-1,-1, // back
            -1,-1, 1,  1,-1, 1,  1,1, 1,  1,1, 1, -1,1, 1, -1,-1, 1,   // front
            -1,1, 1, -1,1,-1, -1,-1,-1, -1,-1,-1, -1,-1,1, -1,1,1,      // left
            1,1, 1, 1,1,-1, 1,-1,-1, 1,-1,-1, 1,-1,1, 1,1,1,           // right
            -1,1,-1, 1,1,-1, 1,1,1, 1,1,1, -1,1,1, -1,1,-1,            // top
            -1,-1,-1, 1,-1,-1, 1,-1,1, 1,-1,1, -1,-1,1, -1,-1,-1      // bottom
        };


        // OpenGL buffer setup
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void draw(GLuint shaderProgram, const glm::mat4& model,
              const glm::mat4& view, const glm::mat4& projection) {
        glUseProgram(shaderProgram);
        glm::mat4 scaledModel = glm::scale(model, size);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(scaledModel));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
};
