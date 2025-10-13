#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "marble.h"
#include "shader_utils.h"

#include <filesystem>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

namespace fs = std::filesystem;

std::string getShaderPath(const std::string& fileName) {
    fs::path exePath = fs::current_path(); // or use argv[0] for more control
    fs::path shaderPath = exePath / "shaders" / fileName;
    std::cout << "Looking for shader at: " << shaderPath << std::endl;
    return shaderPath.string();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Marble Run - Single Marble", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return -1;
    }
    
    GLuint marbleProgram = createShaderProgram(getShaderPath("marble.vert"), getShaderPath("marble.frag"));

    glEnable(GL_DEPTH_TEST);

    Marble marble(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.7f, 1.0f), 0.5f);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        marble.draw(marbleProgram, view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
