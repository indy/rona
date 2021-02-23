// Copyright (C) 2021 Inderjit Gill <email@indy.io>

// This file is part of Rona

// Rona is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Rona is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.

// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef COLOUR_H
#define COLOUR_H

// |--------+-----------+-------------+-------------|
// | format | element 0 | element 1   | element 2   |
// |--------+-----------+-------------+-------------|
// | sRGB   | R 0..1    | G 0..1      | B 0..1      |
// | HSLuv  | H 0..360  | S 0..100    | L 0..100    |
// | OKLAB  | L         | A           | B           |
// | HSL    | H 0..360  | S 0..1      | L 0..1      |
// | LAB    | L 0..100  | A -128..128 | B -128..128 |
// |--------+-----------+-------------+-------------|

// important colour spaces
//
// sRGB : what the world uses, avoid any colour manipulation in RGB space
// HSLuv: for humans to manipulate colours
// OKLAB: for my code to manipulate colours

// secondary colour spaces
//
// HSL
// HSV
// LAB

// internal colour spaces
//
// XYZ
// RGB (linear RGB, could be used externally with appropriate shaders)

typedef enum {
  ColourFormat_sRGB, // sRGB
  ColourFormat_RGB,  // linear RGB
  ColourFormat_HSLuv,
  ColourFormat_OKLAB,
  ColourFormat_HSL,
  ColourFormat_HSV,
  ColourFormat_LAB,
  ColourFormat_XYZ
} ColourFormat;

typedef struct {
  ColourFormat format;
  f32          element[4];
} Colour;

Colour colour_make(ColourFormat format, f32 e0, f32 e1, f32 e2, f32 alpha);
void   colour_set(Colour* out, ColourFormat format, f32 e0, f32 e1, f32 e2, f32 alpha);
void   colour_from(Colour* out, ColourFormat out_format, ColourFormat in_format, f32 e0, f32 e1, f32 e2,
                   f32 alpha);

Colour* colour_clone_as(Colour* out, Colour* in, ColourFormat new_format);
void    vec4_from_colour(Vec4* out, Colour* in);

#endif /* COLOUR_H */
