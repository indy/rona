#version 330 core

uniform sampler2D tilesheet;

in vec2 frag_tex_coord;
in vec4 frag_colour_fg;
in vec4 frag_colour_bg;

out vec4 out_colour;

void main() {
  vec4 texture_colour = texture(tilesheet, frag_tex_coord);

  // vec4 res = mix(frag_colour_bg, frag_colour_fg, texture_colour.aaaa);
  // out_colour = res;

  out_colour = texture_colour;
}
