#version 410 core
out vec4 FragColor;
uniform vec3 marbleColor;

void main() {
    FragColor = vec4(marbleColor, 1.0);
}
