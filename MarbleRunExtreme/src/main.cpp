// Standard headers
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <random>

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
#include "physics.h"
#include "marble_entity.h"
#include "box_entity.h"
#include "track_utils.h"

// Bullet
#include <bullet/btBulletDynamicsCommon.h>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
int winWidth = SCR_WIDTH, winHeight = SCR_HEIGHT;

Camera camera(glm::vec3(4.0f, 12.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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
    GLuint trackProgram = createShaderProgram("shaders/track.vert", "shaders/track.frag");

    // ---------------- Scene Objects ----------------
    Skybox skybox(SKYBOX_IMAGE);

    // ---------------- Bullet Physics ----------------
    PhysicsWorld physics;
    //physics.addGround();
    
    // ---------------- Track Setup ----------------
    std::vector<MeshEntity> meshTrack = buildCurvedMeshTrack(physics);

    // ---------------- Random Marble Setup ----------------
    std::vector<MarbleEntity> marbles;
    MarbleEntity* playerMarble = nullptr;

    // Random engine setup
    std::random_device rd;
    std::mt19937 gen(rd());

    // Random property distributions
    std::uniform_real_distribution<float> colorDist(0.2f, 1.0f);
    std::uniform_real_distribution<float> radiusDist(0.3f, 0.7f);
    std::uniform_real_distribution<float> massDist(0.5f, 4.0f);

    // Random position offsets relative to player spawn
    std::uniform_real_distribution<float> offsetXZ(-2.0f, 2.0f);
    std::uniform_real_distribution<float> offsetY(-1.0f, 1.0f);

    const int NUM_MARBLES = 50;

    // Player marble spawn
    glm::vec3 spawnCenter(31.0f, 26.0f, 1.0f);
    marbles.emplace_back(spawnCenter,
                         glm::vec3(0.2f, 0.6f, 1.0f),
                         0.5f, 1.0f, physics);
    playerMarble = &marbles.back();

    // Generate random marbles around player spawn
    for (int i = 0; i < NUM_MARBLES - 1; ++i) {
        glm::vec3 pos(
            spawnCenter.x + offsetXZ(gen),
            spawnCenter.y + offsetY(gen),
            spawnCenter.z + offsetXZ(gen)
        );
        glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
        float radius = radiusDist(gen);
        float mass = massDist(gen);

        marbles.emplace_back(pos, color, radius, mass, physics);
    }

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

        // Update all marbles
        for (auto& m : marbles)
            m.updateFromPhysics(physics);

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
        
        glUseProgram(trackProgram);
        glUniform3fv(glGetUniformLocation(trackProgram, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(trackProgram, "viewPos"), 1, glm::value_ptr(camera.position));
        glUniform3fv(glGetUniformLocation(trackProgram, "objectColor"), 1, glm::value_ptr(glm::vec3(1.0f, 0.5f, 0.2f))); // orange

        // Draw all marbles
        for (auto& m : marbles)
            m.renderable.draw(marbleProgram, view, projection);
        
        glUseProgram(trackProgram);
        for (auto& seg : meshTrack)
            seg.draw(trackProgram, view, projection);

        // Draw skybox
        skybox.draw(view, projection, skyboxProgram);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}



// https://www.youtube.com/watch?v=keA92Gse2v8
