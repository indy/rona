#version 330 core

in vec4 outColour;
in vec2 outTexCoord;

uniform sampler2D ourTexture;

out vec4 frag_colour;

uniform vec4 colour;

void main() {
  // frag_colour = ourColour;

//  frag_colour = colour;

  // if (outColour.x < 1.0) {
  //     frag_colour = vec4(0.0, 1.0, 0.0, 1.0);
  // } else {
  //     frag_colour = vec4(0.0, 0.0, 1.0, 1.0);
  // }

  // frag_colour = outColour;

//  frag_colour = colour;
//frag_colour = vec4(0.0, 1.0, 1.0, 1.0);

  vec4 texture_colour = texture(ourTexture, outTexCoord);
  frag_colour = vec4(texture_colour.r, texture_colour.g, texture_colour.b, texture_colour.a);
  // frag_colour = vec4(0.1, 0.0, 0.0, 0.8);
}
