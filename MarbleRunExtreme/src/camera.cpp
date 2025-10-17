#include "camera.h"
#include <iostream>
#include <cmath>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : position(position), worldUp(up), yaw(yaw), pitch(pitch),
      front(glm::vec3(0.0f, 0.0f, -1.0f)),
      movementSpeed(2.5f), mouseSensitivity(0.1f)
{
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(int key, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    if (key == GLFW_KEY_W) position += front * velocity;
    if (key == GLFW_KEY_S) position -= front * velocity;
    if (key == GLFW_KEY_A) position -= right * velocity;
    if (key == GLFW_KEY_D) position += right * velocity;
    if (key == GLFW_KEY_Q) position += up * velocity;
    if (key == GLFW_KEY_E) position -= up * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if (constrainPitch) {
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 worldFront;
    worldFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    worldFront.y = sin(glm::radians(pitch));
    worldFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(worldFront);
    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
}

void Camera::reset(glm::vec3 newPos) {
    position = newPos;
    yaw = -90.0f;
    pitch = 0.0f;
    updateCameraVectors();
}

// Input Handling
static bool cursorVisible = false;
static bool spacePressedLastFrame = false;

void processInput(GLFWwindow* window, Camera& camera, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.processKeyboard(GLFW_KEY_W, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.processKeyboard(GLFW_KEY_S, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.processKeyboard(GLFW_KEY_A, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.processKeyboard(GLFW_KEY_D, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) camera.processKeyboard(GLFW_KEY_Q, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) camera.processKeyboard(GLFW_KEY_E, deltaTime);

    // Quit program
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Toggle cursor visibility with Space
    bool spacePressedThisFrame = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    if (spacePressedThisFrame && !spacePressedLastFrame) {
        cursorVisible = !cursorVisible;
        glfwSetInputMode(window, GLFW_CURSOR, cursorVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
    spacePressedLastFrame = spacePressedThisFrame;
}

// Mouse Callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float lastX = 400.0f;
    static float lastY = 300.0f;
    static bool firstMouse = true;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    Camera* cam = (Camera*)glfwGetWindowUserPointer(window);
    cam->processMouseMovement(xoffset, yoffset);
}
