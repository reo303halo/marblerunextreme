#include "camera.h"
#include <iostream>
#include <cmath>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Position(position), WorldUp(up), Yaw(yaw), Pitch(pitch),
      Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      MovementSpeed(2.5f), MouseSensitivity(0.1f)
{
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::processKeyboard(int key, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (key == GLFW_KEY_W) Position += Front * velocity;
    if (key == GLFW_KEY_S) Position -= Front * velocity;
    if (key == GLFW_KEY_A) Position -= Right * velocity;
    if (key == GLFW_KEY_D) Position += Right * velocity;
    if (key == GLFW_KEY_Q) Position += Up * velocity;
    if (key == GLFW_KEY_E) Position -= Up * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    if (constrainPitch) {
        if (Pitch > 89.0f) Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up    = glm::normalize(glm::cross(Right, Front));
}

void Camera::reset(glm::vec3 newPos) {
    Position = newPos;
    Yaw = -90.0f;
    Pitch = 0.0f;
    updateCameraVectors();
}

// ===== INPUT HANDLING =====

static bool cursorVisible = false;
static bool spacePressedLastFrame = false;

void processInput(GLFWwindow* window, Camera& camera, float deltaTime) {
    // Movement keys
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

// ===== MOUSE CALLBACK =====

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
