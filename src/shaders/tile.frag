#version 330 core

in vec2 outTexCoord;

uniform sampler2D tilesheet;
uniform vec4 colour_fg;
uniform vec4 colour_bg;

out vec4 frag_colour;

void main() {
  vec4 texture_colour = texture(tilesheet, outTexCoord);

  vec4 res = mix(colour_bg, colour_fg, texture_colour.aaaa);
  frag_colour = res;
}
