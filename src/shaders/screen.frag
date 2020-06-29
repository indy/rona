#version 330 core

in vec2 texture_coord;

uniform sampler2D screen_texture;

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

// // cross hatching
// //
// float hatchOffsetY = 5.0;
// float lumThreshold01 = 0.9;
// float lumThreshold02 = 0.7;
// float lumThreshold03 = 0.5;
// float lumThreshold04 = 0.3;

// // bloom
// //
// const vec2 size = vec2(640, 360);   // render size
// const float samples = 5.0;          // pixels per axis; higher = bigger glow, worse performance
// const float quality = 1.1; 	        // lower = smaller glow, better quality

void main() {
  vec4 texture_colour = texture(screen_texture, texture_coord);
  frag_colour = vec4(linear_to_srgb(texture_colour.rgb), 1.0);


  // --------------------------------------------------------------------------------
  // // bloom
  // //
  // vec4 sum = vec4(0);
  // vec2 sizeFactor = vec2(1)/size*quality;
  // // Texel color fetching from texture screen_texture
  // vec4 source = texture(screen_texture, texture_coord);
  // const int range = 2;            // should be = (samples - 1)/2;
  // for (int x = -range; x <= range; x++)
  // {
  //   for (int y = -range; y <= range; y++)
  //   {
  //     sum += texture(screen_texture, texture_coord + vec2(x, y)*sizeFactor);
  //   }
  // }
  // // Calculate final fragment color
  // vec4 res = ((sum/(samples*samples)) + source);
  // frag_colour = vec4(linear_to_srgb(res.rgb), 1.0);

  // --------------------------------------------------------------------------------
  // grayscale
  //
  // vec3 srgb = linear_to_srgb(texture_colour.rgb);
  // // convert texel color to grayscale using NTSC conversion weights
  // float gray = dot(srgb, vec3(0.299, 0.587, 0.114));
  // frag_colour = vec4(gray, gray, gray, 1.0);

  // --------------------------------------------------------------------------------
  // cross hatching
  //
  // vec3 tc = vec3(1.0, 1.0, 1.0);
  //   float lum = length(linear_to_srgb(texture_colour.rgb));
  //   if (lum < lumThreshold01)
  //   {
  //       if (mod(gl_FragCoord.x + gl_FragCoord.y, 10.0) == 0.0) tc = vec3(0.0, 0.0, 0.0);
  //   }
  //   if (lum < lumThreshold02)
  //   {
  //       if (mod(gl_FragCoord.x - gl_FragCoord.y, 10.0) == 0.0) tc = vec3(0.0, 0.0, 0.0);
  //   }
  //   if (lum < lumThreshold03)
  //   {
  //       if (mod(gl_FragCoord.x + gl_FragCoord.y - hatchOffsetY, 10.0) == 0.0) tc = vec3(0.0, 0.0, 0.0);
  //   }
  //   if (lum < lumThreshold04)
  //   {
  //       if (mod(gl_FragCoord.x - gl_FragCoord.y - hatchOffsetY, 10.0) == 0.0) tc = vec3(0.0, 0.0, 0.0);
  //   }
  //   frag_colour = vec4(tc, 1.0);

  // --------------------------------------------------------------------------------
  // // dream vision
  // //
  // vec4 color = texture(screen_texture, texture_coord);
  // color += texture(screen_texture, texture_coord + 0.001);
  // color += texture(screen_texture, texture_coord + 0.003);
  // color += texture(screen_texture, texture_coord + 0.005);
  // color += texture(screen_texture, texture_coord + 0.007);
  // color += texture(screen_texture, texture_coord + 0.009);
  // color += texture(screen_texture, texture_coord + 0.011);
  // color += texture(screen_texture, texture_coord - 0.001);
  // color += texture(screen_texture, texture_coord - 0.003);
  // color += texture(screen_texture, texture_coord - 0.005);
  // color += texture(screen_texture, texture_coord - 0.007);
  // color += texture(screen_texture, texture_coord - 0.009);
  // color += texture(screen_texture, texture_coord - 0.011);
  // color.rgb = vec3((color.r + color.g + color.b)/3.0);
  // color = color/9.5;
  // frag_colour = vec4(linear_to_srgb(color.rgb), 1.0);
}
