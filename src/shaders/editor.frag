#version 330 core

precision mediump float;

uniform sampler2D nuklear_texture;

in vec2 frag_tex_coord;
in vec4 frag_colour;

out vec4 Out_Color;

void main(){
  Out_Color = frag_colour * texture(nuklear_texture, frag_tex_coord.st);
};
