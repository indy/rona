#version 330 core

layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec2 in_tex_coord;
layout(location = 2) in vec4 in_colour_fg;
layout(location = 3) in vec4 in_colour_bg;

uniform mat4 proj_matrix;
uniform vec3 pos;

out vec2 frag_tex_coord;
out vec4 frag_colour_fg;
out vec4 frag_colour_bg;

void main() {
  gl_Position = proj_matrix * vec4(pos.x + in_pos.x, pos.y + in_pos.y, pos.z, 1.0);

  frag_tex_coord = in_tex_coord;
  frag_colour_fg = in_colour_fg;
  frag_colour_bg = in_colour_bg;
}
