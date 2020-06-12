#version 330 core

layout(location = 0) in vec4 inPosition;

uniform mat4 proj_matrix;
uniform vec3 pos;

void main() {
  gl_Position = proj_matrix * (inPosition + vec4(pos.x, pos.y, pos.z, 0));
}
