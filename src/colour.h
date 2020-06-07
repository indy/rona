#ifndef COLOUR_H
#define COLOUR_H

// |--------+-----------+-------------+-------------|
// | format | element 0 | element 1   | element 2   |
// |--------+-----------+-------------+-------------|
// | sRGB   | R 0..1    | G 0..1      | B 0..1      |
// | HSL    | H 0..360  | S 0..1      | L 0..1      |
// | HSLuv  | H 0..360  | S 0..100    | L 0..100    |
// | LAB    | L 0..100  | A -128..128 | B -128..128 |
// |--------+-----------+-------------+-------------|

typedef enum {
  ColourFormat_sRGB,            // sRGB
  ColourFormat_RGB,             // linear RGB
  ColourFormat_HSL,
  ColourFormat_HSLuv,
  ColourFormat_LAB,
  ColourFormat_HSV,
  ColourFormat_XYZ
} ColourFormat;

typedef struct {
  ColourFormat format;
  f32          element[4];
} Colour;

void colour_set(Colour* out, ColourFormat format, f32 e0, f32 e1, f32 e2, f32 alpha);
void colour_from(Colour* out, ColourFormat out_format, ColourFormat in_format, f32 e0, f32 e1, f32 e2, f32 alpha);

Colour* colour_clone_as(Colour* out, Colour* in, ColourFormat new_format);


#endif /* COLOUR_H */
