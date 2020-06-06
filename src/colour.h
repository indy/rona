#pragma once

// |--------+-----------+-------------+-------------|
// | format | element 0 | element 1   | element 2   |
// |--------+-----------+-------------+-------------|
// | RGB    | R 0..1    | G 0..1      | B 0..1      |
// | HSL    | H 0..360  | S 0..1      | L 0..1      |
// | HSLuv  | H 0..360  | S 0..100    | L 0..100    |
// | LAB    | L 0..100  | A -128..128 | B -128..128 |
// |--------+-----------+-------------+-------------|

typedef enum { RGB, HSL, HSLuv, LAB, HSV, XYZ } colourFormat;

typedef struct {
  colourFormat format;
  f32         element[4];
} colour;

void colour_set(colour* out, colourFormat format, f32 e0, f32 e1, f32 e2, f32 alpha);
colour* colour_clone_as(colour* out, colour* in, colourFormat new_format);
