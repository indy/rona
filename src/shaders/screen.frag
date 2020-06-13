#version 330 core

in vec2 texture_coord;

uniform sampler2D sampler;

out vec4 frag_colour;

vec3 linear_to_srgb(vec3 linear) {
    float a = 0.055;
    float b = 0.0031308;
    vec3 srgb_lo = 12.92 * linear;
    vec3 srgb_hi = (1.0 + a) * pow(linear, vec3(1.0/2.4)) - vec3(a);
    return vec3(
        linear.r > b ? srgb_hi.r : srgb_lo.r,
        linear.g > b ? srgb_hi.g : srgb_lo.g,
        linear.b > b ? srgb_hi.b : srgb_lo.b);
}

void main() {
  vec4 texture_colour = texture(sampler, texture_coord);

  frag_colour = vec4(linear_to_srgb(texture_colour.rgb), 1.0);
}
