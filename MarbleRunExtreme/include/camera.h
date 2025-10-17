#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;
    float movementSpeed;
    float mouseSensitivity;

    Camera(glm::vec3 position, glm::vec3 up, float yaw = -90.0f, float pitch = 0.0f);

    glm::mat4 getViewMatrix() const;

    void processKeyboard(int key, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    void reset(glm::vec3 newPos);

private:
    void updateCameraVectors();
};

void processInput(GLFWwindow* window, Camera& camera, float deltaTime);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
