#version 330 core

layout(location = 0) in vec2 inPosition;

uniform mat4 proj_matrix;
uniform vec3 pos;

void main() {
  gl_Position = proj_matrix * vec4(pos.x + inPosition.x, pos.y + inPosition.y, pos.z, 1.0);

}
