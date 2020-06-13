#version 330 core

in vec2 texture_coord;

uniform sampler2D sampler;

out vec4 frag_colour;

void main() {

  vec4 texture_colour = texture(sampler, texture_coord);

  frag_colour = texture_colour;

//  frag_colour = colour;
}
