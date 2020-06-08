#version 330 core

layout(location = 0) in vec4 inPosition;

out vec4 vertex_colour;

void main() {
  gl_Position = inPosition;
  vertex_colour = vec4(0.6, 0.1, 0.3, 1.0);
}
