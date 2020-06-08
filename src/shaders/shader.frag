#version 330 core

out vec4 frag_colour;

in vec4 vertex_colour;

void main() {
  frag_colour = vertex_colour;
}
