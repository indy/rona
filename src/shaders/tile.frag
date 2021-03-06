#version 330 core

uniform sampler2D tilesheet;

in vec2 frag_tex_coord;

out vec4 out_colour;

void main() {
  vec4 texture_colour = texture(tilesheet, frag_tex_coord);
  out_colour = texture_colour;
}
