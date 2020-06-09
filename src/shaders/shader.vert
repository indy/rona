#version 330 core

layout(location = 0) in vec4 inPosition;

uniform mat4 proj_matrix;

void main() {
  gl_Position = proj_matrix * inPosition;
}
