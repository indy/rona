#version 330 core

layout(location = 0) in vec4 inPosition;

uniform mat4 proj_matrix;
uniform vec2 pos;

void main() {
  gl_Position = proj_matrix * (inPosition + vec4(pos.x, pos.y, 0, 0));
}
