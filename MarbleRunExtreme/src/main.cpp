// Standard headers
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

// OpenGL / GLM
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// My headers
#include "marble.h"
#include "shader_utils.h"
#include "skybox.h"
#include "camera.h"

// Physics
#include "physics.h"
#include <bullet/btBulletDynamicsCommon.h>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
int winWidth = SCR_WIDTH, winHeight = SCR_HEIGHT;

Camera camera(glm::vec3(0.0f, 2.0f, 8.0f), glm::vec3(0.0f, 1.0f, 0.0f));
float deltaTime = 0.0f;
float lastFrame = 0.0f;

const std::string SKYBOX_IMAGE = "assets/skybox/red_sky.png";

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    winWidth = width;
    winHeight = height;
}

int main() {
    // ---------------- GLFW / OpenGL Init ----------------
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Marble Run Extreme", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(window, &camera);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // ---------------- Shaders ----------------
    GLuint skyboxProgram = createShaderProgram("shaders/skybox.vert", "shaders/skybox.frag");
    GLuint marbleProgram = createShaderProgram("shaders/marble.vert", "shaders/marble.frag");

    // ---------------- Scene Objects ----------------
    Skybox skybox(SKYBOX_IMAGE);
    Marble marble(glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.2f, 0.7f, 1.0f),
                  0.5f);
    Marble marble2(glm::vec3(0.0f, 10.0f, 0.0f),
                   glm::vec3(1.0f, 0.2f, 0.2f),
                   0.5f);

    // ---------------- Bullet Physics Setup ----------------
    PhysicsWorld physics;
    physics.addGround();
    btRigidBody* sphereBody1 = physics.addSphere(0.5f, glm::vec3(0, 5, 0));
    btRigidBody* sphereBody2 = physics.addSphere(0.5f, glm::vec3(0.1, 10, 0.1));


    // ---------------- Light ----------------
    glm::vec3 lightPos(2.0f, 2.0f, 2.0f);

    // ---------------- Render Loop ----------------
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, camera, deltaTime);

        // Step physics
        physics.step(deltaTime);

        // Update marbles from Bullet
        marble.position = physics.getObjectPosition(sphereBody1);
        marble2.position = physics.getObjectPosition(sphereBody2);
        
        // Camera follows marble2
        //camera.position = glm::vec3(marble2.position.x, marble2.position.y + 2.0f, marble2.position.z + 8.0f);

        // Clear screen
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            (float)winWidth / (float)winHeight, 0.1f, 100.0f);

        // Update light
        float lightSpeed = 0.2f;
        lightPos.x = 2.0f * sin(glfwGetTime() * lightSpeed);
        lightPos.z = 2.0f * cos(glfwGetTime() * lightSpeed);

        glUseProgram(marbleProgram);
        glUniform3fv(glGetUniformLocation(marbleProgram, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(marbleProgram, "viewPos"), 1, glm::value_ptr(camera.position));

        // Draw marble and skybox
        marble.draw(marbleProgram, view, projection);
        marble2.draw(marbleProgram, view, projection);
        skybox.draw(view, projection, skyboxProgram);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
