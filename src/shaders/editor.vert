#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in vec4 colour;

uniform mat4 proj_matrix;

out vec2 frag_tex_coord;
out vec4 frag_colour;

void main() {
  frag_tex_coord = tex_coord;
  frag_colour = colour;
  gl_Position = proj_matrix * vec4(pos.xy, 0, 1);
};
