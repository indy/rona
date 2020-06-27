#version 330 core

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec4 inColour;
layout(location = 2) in vec2 inTexCoord;

uniform mat4 proj_matrix;
uniform vec3 pos;

out vec4 outColour;
out vec2 outTexCoord;

void main() {
  gl_Position = proj_matrix * vec4(pos.x + inPosition.x, pos.y + inPosition.y, pos.z, 1.0);
  outColour = inColour;
  outTexCoord = inTexCoord;
}
