#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;
    float MovementSpeed;
    float MouseSensitivity;

    Camera(glm::vec3 position, glm::vec3 up, float yaw = -90.0f, float pitch = 0.0f);

    glm::mat4 getViewMatrix() const;

    void processKeyboard(int key, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    // Optional helper to reset position
    void reset(glm::vec3 newPos);

private:
    void updateCameraVectors();
};

// Utility functions to handle input & mouse
void processInput(GLFWwindow* window, Camera& camera, float deltaTime);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
