#include "marble_run.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace marble_run {

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera translation
float camX = 0.0f, camY = 0.0f, camZ = -3.0f; // Start away from origin

// Pyramid rotation
float rotationAngle = 0.0f;

float cubeRotation = 0.0f;


// ---------- SHADERS ----------
// Pyramid shaders -- fragment shader animates side colors using cosine(time)
const char* pyramidVertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec3 in_position;

    uniform mat4 transform;

    out vec3 v_pos;

    void main() {
        v_pos = in_position;
        gl_Position = transform * vec4(in_position, 1.0);
    }
)";

const char* pyramidFragmentShaderSource = R"(
    #version 330 core
    out vec4 frag_color;

    in vec3 v_pos;
    uniform float time;

    void main() {
        if (v_pos.y <= 0.001) {
            frag_color = vec4(0.15, 0.15, 0.15, 1.0); // base stays dark
        } else {
            vec3 animated = 0.5 + 0.5 * cos(vec3(time) + v_pos * 5.0);
            frag_color = vec4(animated, 1.0);
        }
    }
)";

// Cube shaders -- vertex shader flattens z and computes color from x,y
const char* cubeVertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec3 in_position;

    uniform mat4 transform;

    out vec3 v_color;

    void main() {
        float flattenFactor = 0.2;
        vec3 pos = in_position;
        pos.z *= flattenFactor;

        v_color = vec3(pos.x + 0.5, pos.y + 0.5, 0.5);

        gl_Position = transform * vec4(pos, 1.0);
    }
)";

const char* cubeFragmentShaderSource = R"(
    #version 330 core
    out vec4 frag_color;
    in vec3 v_color;
    void main() {
        frag_color = vec4(v_color, 1.0);
    }
)";


void checkCompileErrors(unsigned int shader, const std::string& type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: "
                      << type << "\n" << infoLog << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: "
                      << type << "\n" << infoLog << std::endl;
        }
    }
}


int run() {
    if (!glfwInit()) {
        std::cerr << "Error initializing GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Task 4 - Shaders", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        glfwTerminate();
        return -1;
    }
    glGetError(); // clear GLEW error

    // --- Compile pyramid shader program ---
    GLuint pv = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(pv, 1, &pyramidVertexShaderSource, NULL);
    glCompileShader(pv);
    checkCompileErrors(pv, "PYRAMID_VERTEX");

    GLuint pf = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(pf, 1, &pyramidFragmentShaderSource, NULL);
    glCompileShader(pf);
    checkCompileErrors(pf, "PYRAMID_FRAGMENT");

    GLuint pyramidProgram = glCreateProgram();
    glAttachShader(pyramidProgram, pv);
    glAttachShader(pyramidProgram, pf);
    glLinkProgram(pyramidProgram);
    checkCompileErrors(pyramidProgram, "PROGRAM");

    // --- Compile cube shader program ---
    GLuint cv = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(cv, 1, &cubeVertexShaderSource, NULL);
    glCompileShader(cv);
    checkCompileErrors(cv, "CUBE_VERTEX");

    GLuint cf = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(cf, 1, &cubeFragmentShaderSource, NULL);
    glCompileShader(cf);
    checkCompileErrors(cf, "CUBE_FRAGMENT");

    GLuint cubeProgram = glCreateProgram();
    glAttachShader(cubeProgram, cv);
    glAttachShader(cubeProgram, cf);
    glLinkProgram(cubeProgram);
    checkCompileErrors(cubeProgram, "PROGRAM");

    // ---------- Geometry ----------
    // Pyramid
    float pyramidVertices[] = {
        -0.5f, 0.0f, -0.5f,
         0.5f, 0.0f, -0.5f,
         0.5f, 0.0f,  0.5f,
        -0.5f, 0.0f,  0.5f,
         0.0f, 0.8f,  0.0f
    };
    unsigned int pyramidIndices[] = {
        0, 1, 2, 0, 2, 3,
        0, 1, 4, 1, 2, 4,
        2, 3, 4, 3, 0, 4
    };

    unsigned int VAO_pyr, VBO_pyr, EBO_pyr;
    glGenVertexArrays(1, &VAO_pyr);
    glGenBuffers(1, &VBO_pyr);
    glGenBuffers(1, &EBO_pyr);
    glBindVertexArray(VAO_pyr);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_pyr);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_pyr);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyramidIndices), pyramidIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Cube
    float cubeVertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f
    };
    unsigned int cubeIndices[] = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        0, 4, 7, 0, 7, 3,
        1, 5, 6, 1, 6, 2,
        3, 2, 6, 3, 6, 7,
        0, 1, 5, 0, 5, 4
    };

    unsigned int VAO_cube, VBO_cube, EBO_cube;
    glGenVertexArrays(1, &VAO_cube);
    glGenBuffers(1, &VBO_cube);
    glGenBuffers(1, &EBO_cube);
    glBindVertexArray(VAO_cube);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_cube);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.18f, 0.05f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(camX, camY, camZ));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // ---- Pyramid ----
        glUseProgram(pyramidProgram);
        glm::mat4 model_p = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 transform_p = projection * view * model_p;
        glUniformMatrix4fv(glGetUniformLocation(pyramidProgram, "transform"), 1, GL_FALSE, glm::value_ptr(transform_p));
        glUniform1f(glGetUniformLocation(pyramidProgram, "time"), (float)glfwGetTime());
        glBindVertexArray(VAO_pyr);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

        // ---- Cube ----
        glUseProgram(cubeProgram);
        glm::mat4 model_c = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
        model_c = glm::rotate(model_c, cubeRotation, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 transform_c = projection * view * model_c;
        glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "transform"), 1, GL_FALSE, glm::value_ptr(transform_c));
        glBindVertexArray(VAO_cube);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        cubeRotation += 0.01f;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteProgram(pyramidProgram);
    glDeleteProgram(cubeProgram);
    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow* window)
{
    float moveSpeed = 0.02f;     // Camera speed
    float rotationSpeed = 0.02f; // Pyramid rotation speed
    
    float cubeRotation = 0.0f;
    cubeRotation += 0.01f;


    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Camera translation
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    camY += moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  camY -= moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) camX += moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  camX -= moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)     camZ += moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)     camZ -= moveSpeed;

    // Pyramid rotation
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)     rotationAngle += rotationSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)     rotationAngle -= rotationSpeed;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
}
