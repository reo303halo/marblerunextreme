#pragma once
#include <string>
#include <GL/glew.h>

std::string loadShaderSource(const std::string& filePath);
GLuint createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
