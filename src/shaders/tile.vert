#version 330 core

layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec2 in_tex_coord;

uniform mat4 proj_matrix;
uniform vec3 pos;

out vec2 frag_tex_coord;

void main() {
  gl_Position = proj_matrix * vec4(pos.x + in_pos.x, pos.y + in_pos.y, pos.z, 1.0);
  frag_tex_coord = in_tex_coord;
}
