#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;

uniform mat4 proj_matrix;

out vec2 texture_coord;

void main() {
  gl_Position = proj_matrix * vec4(pos.x, pos.y, 0.0, 1.0);
  texture_coord = uv;
}
