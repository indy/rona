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

/*
  Note: All colour conversion code assumes that colour64 RGB is in linear space
  However, outside of this file, all colour RGB is in sRGB colour space
  So we perform gamma expansion/correction at the converting from/to stages
*/

typedef struct Colour64 {
  ColourFormat format;
  f64          element[4];
} Colour64;

const f64 ref_u = 0.19783000664283680764;
const f64 ref_v = 0.46831999493879100370;

//  http://www.brucelindbloom.com/index.html?Equations.html
//  http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html

// we're using an sRGB working space with a D65 reference white

// https://uk.mathworks.com/help/images/ref/whitepoint.html
// the D65 whitepoint
#define WHITEPOINT_0 0.9504
#define WHITEPOINT_1 1.0
#define WHITEPOINT_2 1.0888

#define CIE_EPSILON 0.008856
#define CIE_KAPPA 903.3

// intent from the CIE
//
// #define CIE_EPSILON (216.0f / 24389.0f)
// #define CIE_KAPPA (24389.0f / 27.0f)

// RGB to XYZ (M)
// 0.4124564  0.3575761  0.1804375
// 0.2126729  0.7151522  0.0721750
// 0.0193339  0.1191920  0.9503041

// XYZ to RBG (M)^-1
//  3.2404542 -1.5371385 -0.4985314
// -0.9692660  1.8760108  0.0415560
//  0.0556434 -0.2040259  1.0572252

f64 inverse_srgb_companding(f64 component) {
  if (component > 0.0405) {
    return pow((component + 0.055) / 1.055, 2.4);
  } else {
    return (component / 12.92);
  }
}

f64 srgb_companding(f64 a) {
  if (a > 0.0031308) {
    return (1.055 * pow(a, 1.0 / 2.4)) - 0.055;
  } else {
    return a * 12.92;
  }
}

void colour_from(Colour* out, ColourFormat out_format, ColourFormat in_format, f32 e0, f32 e1, f32 e2,
                 f32 alpha) {
  Colour in_col = {in_format, {e0, e1, e2, alpha}};
  colour_clone_as(out, &in_col, out_format);
}

Colour colour_make(ColourFormat format, f32 e0, f32 e1, f32 e2, f32 alpha) {
  Colour res;
  res.format     = format;
  res.element[0] = e0;
  res.element[1] = e1;
  res.element[2] = e2;
  res.element[3] = alpha;

  return res;
}

void colour_set(Colour* out, ColourFormat format, f32 e0, f32 e1, f32 e2, f32 alpha) {
  out->format     = format;
  out->element[0] = e0;
  out->element[1] = e1;
  out->element[2] = e2;
  out->element[3] = alpha;
}

Colour64* colour64_from_colour(Colour64* out, Colour* in) {
  out->format     = in->format;
  out->element[0] = (f64)(in->element[0]);
  out->element[1] = (f64)(in->element[1]);
  out->element[2] = (f64)(in->element[2]);
  out->element[3] = (f64)(in->element[3]);

  return out;
}

Colour* colour_from_colour64(Colour* out, Colour64* in) {
  out->format     = in->format;
  out->element[0] = (f32)(in->element[0]);
  out->element[1] = (f32)(in->element[1]);
  out->element[2] = (f32)(in->element[2]);
  out->element[3] = (f32)(in->element[3]);

  return out;
}

Colour* colour_clone(Colour* out, Colour* in) {
  out->format     = in->format;
  out->element[0] = in->element[0];
  out->element[1] = in->element[1];
  out->element[2] = in->element[2];
  out->element[3] = in->element[3];

  return out;
}

Colour64* xyz_from_rgb(Colour64* col) {
  f64 r = col->element[0];
  f64 g = col->element[1];
  f64 b = col->element[2];

  // multiply by matrix
  // see http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
  // sRGB colour space with D65 reference white
  //
  col->format = ColourFormat_XYZ;
  col->element[0] =
      (r * 0.41239079926595948129) + (g * 0.35758433938387796373) + (b * 0.18048078840183428751);
  col->element[1] =
      (r * 0.21263900587151035754) + (g * 0.71516867876775592746) + (b * 0.07219231536073371500);
  col->element[2] =
      (r * 0.01933081871559185069) + (g * 0.11919477979462598791) + (b * 0.95053215224966058086);

  return col;
}

Colour64* rgb_from_xyz(Colour64* col) {
  f64 xx = col->element[0];
  f64 yy = col->element[1];
  f64 zz = col->element[2];

  f64 r = (xx * 3.24096994190452134377) + (yy * -1.53738317757009345794) + (zz * -0.49861076029300328366);
  f64 g = (xx * -0.96924363628087982613) + (yy * 1.87596750150772066772) + (zz * 0.04155505740717561247);
  f64 b = (xx * 0.05563007969699360846) + (yy * -0.20397695888897656435) + (zz * 1.05697151424287856072);

  col->format     = ColourFormat_RGB;
  col->element[0] = r;
  col->element[1] = g;
  col->element[2] = b;

  return col;
}

f64 axis_to_LAB_component(f64 a) {
  if (a > CIE_EPSILON) {
    return cbrt(a); // cube root
  } else {
    return ((CIE_KAPPA * a) + 16.0) / 116.0;
  }
}

Colour64* lab_from_xyz(Colour64* col) {
  f64 xr = col->element[0] / WHITEPOINT_0;
  f64 yr = col->element[1] / WHITEPOINT_1;
  f64 zr = col->element[2] / WHITEPOINT_2;

  f64 fx = axis_to_LAB_component(xr);
  f64 fy = axis_to_LAB_component(yr);
  f64 fz = axis_to_LAB_component(zr);

  col->format     = ColourFormat_LAB;
  col->element[0] = (116.0 * fy) - 16.0;
  col->element[1] = 500.0 * (fx - fy);
  col->element[2] = 200.0 * (fy - fz);

  return col;
}

i32 max_channel(Colour64* colour) {
  i32 hi = colour->element[0] > colour->element[1] ? 0 : 1;
  return colour->element[2] > colour->element[hi] ? 2 : hi;
}

i32 min_channel(Colour64* colour) {
  i32 low = colour->element[0] < colour->element[1] ? 0 : 1;
  return colour->element[2] < colour->element[low] ? 2 : low;
}

// http://www.rapidtables.com/convert/color/rgb-to-hsl.htm
f64 hue(Colour64* colour, i32 max_chan, f64 chroma) {
  if (chroma == 0.0) {
    return 0.0; // invalid hue
  }

  f64 angle = 0.0;

  switch (max_chan) {
  case 0: // R
    angle = 60.0 * ((f32)fmod((colour->element[1] - colour->element[2]) / chroma, 6.0));
    break;
  case 1: // G
    angle = 60.0 * (((colour->element[2] - colour->element[0]) / chroma) + 2.0);
    break;
  case 2: // B
    angle = 60.0 * (((colour->element[0] - colour->element[1]) / chroma) + 4.0);
    break;
  default:
    break;
  }

  while (angle < 0.0) {
    angle += 360.0;
  }

  return angle;
}

f64 abso(f64 in) {
  return in < 0.0 ? -in : in;
}

Colour64* rgb_from_oklab(Colour64* col) {
  f64 okl = col->element[0];
  f64 oka = col->element[1];
  f64 okb = col->element[2];

  f64 l_ = okl + 0.3963377774 * oka + 0.2158037573 * okb;
  f64 m_ = okl - 0.1055613458 * oka - 0.0638541728 * okb;
  f64 s_ = okl - 0.0894841775 * oka - 1.2914855480 * okb;

  f64 l = l_ * l_ * l_;
  f64 m = m_ * m_ * m_;
  f64 s = s_ * s_ * s_;

  f64 r = 4.0767245293 * l - 3.3072168827 * m + 0.2307590544 * s;
  f64 g = -1.2681437731 * l + 2.6093323231 * m - 0.3411344290 * s;
  f64 b = -0.0041119885 * l - 0.7034763098 * m + 1.7068625689 * s;

  col->format     = ColourFormat_RGB;
  col->element[0] = r;
  col->element[1] = g;
  col->element[2] = b;

  return col;
}

Colour64* oklab_from_rgb(Colour64* col) {
  f64 lr = col->element[0];
  f64 lg = col->element[1];
  f64 lb = col->element[2];

  f64 l = 0.4121656120 * lr + 0.5362752080 * lg + 0.0514575653 * lb;
  f64 m = 0.2118591070 * lr + 0.6807189584 * lg + 0.1074065790 * lb;
  f64 s = 0.0883097947 * lr + 0.2818474174 * lg + 0.6302613616 * lb;

  f64 l_ = cbrt(l);
  f64 m_ = cbrt(m);
  f64 s_ = cbrt(s);

  f64 okl = 0.2104542553 * l_ + 0.7936177850 * m_ - 0.0040720468 * s_;
  f64 oka = 1.9779984951 * l_ - 2.4285922050 * m_ + 0.4505937099 * s_;
  f64 okb = 0.0259040371 * l_ + 0.7827717662 * m_ - 0.8086757660 * s_;

  col->format     = ColourFormat_OKLAB;
  col->element[0] = okl;
  col->element[1] = oka;
  col->element[2] = okb;

  return col;
}

Colour64* oklab_from_xyz(Colour64* col) {
  f64 x = col->element[0];
  f64 y = col->element[1];
  f64 z = col->element[2];

  f64 l = (x * 0.818933) + (y * 0.361866) - (z * 0.1288597);
  f64 m = (x * 0.032984) + (y * 0.929311) + (z * 0.0361456);
  f64 s = (x * 0.048200) + (y * 0.264366) + (z * 0.6338517);

  f64 ll = cbrt(l);
  f64 mm = cbrt(m);
  f64 ss = cbrt(s);

  f64 okl = (ll * 0.2104542553) + (mm * 0.7936177850) - (ss * 0.0040720468);
  f64 oka = (ll * 1.9779984951) - (mm * 2.4285922050) + (ss * 0.4505937099);
  f64 okb = (ll * 0.0259040371) + (mm * 0.7827717662) - (ss * 0.8086757660);

  col->format     = ColourFormat_OKLAB;
  col->element[0] = okl;
  col->element[1] = oka;
  col->element[2] = okb;

  return col;
}

Colour64* xyz_from_oklab(Colour64* col) {
  f64 l = col->element[0];
  f64 a = col->element[1];
  f64 b = col->element[2];

  f64 ll = (l * 0.99999999845051981432) + (a * 0.39633779217376785678) + (b * 0.21580375806075880339);
  f64 aa = (l * 1.0000000088817607767) - (a * 0.1055613423236563494) - (b * 0.063854174771705903402);
  f64 bb = (l * 1.0000000546724109177) - (a * 0.089484182094965759684) - (b * 1.2914855378640917399);

  f64 lll = ll * ll * ll;
  f64 aaa = aa * aa * aa;
  f64 bbb = bb * bb * bb;

  f64 x = (lll * 1.2270135808797242712) - (aaa * 0.55779929373936236039) + (bbb * 0.28125599738846247759);
  f64 y = (lll * -0.040579504210222529446) + (aaa * 1.1122575496728821897) - (bbb * 0.071676512386310776724);
  f64 z = (lll * -0.076380979002443348587) - (aaa * 0.42148211232782669384) + (bbb * 1.5861632836188583913);

  col->format     = ColourFormat_XYZ;
  col->element[0] = x;
  col->element[1] = y;
  col->element[2] = z;

  return col;
}

// http://www.rapidtables.com/convert/color/rgb-to-hsl.htm
Colour64* hsl_from_rgb(Colour64* col) {
  i32 min_ch  = min_channel(col);
  f64 min_val = col->element[min_ch];

  i32 max_ch  = max_channel(col);
  f64 max_val = col->element[max_ch];

  f64 delta = max_val - min_val;

  f64 h = hue(col, max_ch, delta);

  f64 lightness = 0.5 * (min_val + max_val);

  f64 saturation;
  if (delta == 0.0) {
    saturation = 0.0;
  } else {
    saturation = delta / (1.0 - abso((2.0 * lightness) - 1.0));
  }

  col->format     = ColourFormat_HSL;
  col->element[0] = h;
  col->element[1] = saturation;
  col->element[2] = lightness;

  return col;
}

Colour64* hsv_from_rgb(Colour64* col) {
  i32 min_ch  = min_channel(col);
  f64 min_val = col->element[min_ch];

  i32 max_ch  = max_channel(col);
  f64 max_val = col->element[max_ch];

  f64 chroma = max_val - min_val;
  f64 h      = hue(col, max_ch, chroma);
  // bool valid_hue = (chroma != 0.0);

  f64 value = max_val;

  f64 saturation;
  if (chroma == 0.0) {
    saturation = 0.0;
  } else {
    saturation = chroma / value;
  }

  col->format     = ColourFormat_HSV;
  col->element[0] = h;
  col->element[1] = saturation;
  col->element[2] = value;

  // TODO: set valid_hue
  // return col.set('valid_hue', valid_hue);

  return col;
}

Colour64* rgb_from_chm(Colour64* col, f64 chroma, f64 h, f64 m) {
  // todo: validhue test
  //
  // if (c.get('validHue') === undefined) {
  // return construct(Format.RGB, [m, m, m, element(c, ALPHA)]);
  //}

  f64 hprime = h / 60.0;
  f64 x      = chroma * (1.0 - abso((f64)fmod(hprime, 2.0) - 1.0));
  f64 r      = 0.0;
  f64 g      = 0.0;
  f64 b      = 0.0;

  if (hprime < 1.0) {
    r = chroma;
    g = x;
    b = 0.0;
  } else if (hprime < 2.0) {
    r = x;
    g = chroma;
    b = 0.0;
  } else if (hprime < 3.0) {
    r = 0.0;
    g = chroma;
    b = x;
  } else if (hprime < 4.0) {
    r = 0.0;
    g = x;
    b = chroma;
  } else if (hprime < 5.0) {
    r = x;
    g = 0.0;
    b = chroma;
  } else if (hprime < 6.0) {
    r = chroma;
    g = 0.0;
    b = x;
  }

  col->format     = ColourFormat_RGB;
  col->element[0] = r + m;
  col->element[1] = g + m;
  col->element[2] = b + m;

  return col;
}

Colour64* rgb_from_hsl(Colour64* col) {
  f64 h      = col->element[0];
  f64 s      = col->element[1];
  f64 l      = col->element[2];
  f64 chroma = (1.0 - abso((2.0 * l) - 1.0)) * s;
  f64 m      = l - (0.5 * chroma);

  // todo: set validhue
  // f64 col = c.set('validHue', true);

  return rgb_from_chm(col, chroma, h, m);
}

f64 lab_component_to_axis(f64 l) {
  if (pow(l, 3.0) > CIE_EPSILON) {
    return pow(l, 3.0);
  } else {
    return ((116.0 * l) - 16.0) / CIE_KAPPA;
  }
}

Colour64* xyz_from_lab(Colour64* col) {
  f64 fy = (col->element[0] + 16.0) / 116.0;
  f64 fz = fy - (col->element[2] / 200.0);
  f64 fx = (col->element[1] / 500.0) + fy;

  f64 xr = lab_component_to_axis(fx);
  f64 yr = 0.0;
  if (col->element[0] > (CIE_EPSILON * CIE_KAPPA)) {
    yr = ((col->element[0] + 16.0) / 116.0);
    yr = yr * yr * yr;
  } else {
    yr = col->element[0] / CIE_KAPPA;
  }
  f64 zr = lab_component_to_axis(fz);

  col->format     = ColourFormat_XYZ;
  col->element[0] = WHITEPOINT_0 * xr;
  col->element[1] = WHITEPOINT_1 * yr;
  col->element[2] = WHITEPOINT_2 * zr;

  return col;
}

Colour64* rgb_from_hsv(Colour64* col) {
  f64 h      = col->element[0];
  f64 s      = col->element[1];
  f64 v      = col->element[2];
  f64 chroma = v * s;
  f64 m      = v - chroma;

  return rgb_from_chm(col, chroma, h, m);
}

// the luv and hsluv code is based on https://github.com/hsluv/hsluv-c
// which uses the MIT License:

// # The MIT License (MIT)

// Copyright © 2015 Alexei Boronine (original idea, JavaScript implementation)
// Copyright © 2015 Roger Tallada (Obj-C implementation)
// Copyright © 2017 Martin Mitáš (C implementation, based on Obj-C
// implementation)

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the “Software”),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

typedef struct {
  f64 a;
  f64 b;
} Bounds;

void get_bounds(f64 l, Bounds bounds[6]) {
  f64 tl   = l + 16.0;
  f64 sub1 = (tl * tl * tl) / 1560896.0;
  f64 sub2 = sub1 > CIE_EPSILON ? sub1 : (l / CIE_KAPPA);
  int channel;
  int t;

  f64 m[3][3];
  m[0][0] = 3.24096994190452134377;
  m[0][1] = -1.53738317757009345794;
  m[0][2] = -0.49861076029300328366;
  m[1][0] = -0.96924363628087982613;
  m[1][1] = 1.87596750150772066772;
  m[1][2] = 0.04155505740717561247;
  m[2][0] = 0.05563007969699360846;
  m[2][1] = -0.20397695888897656435;
  m[2][2] = 1.05697151424287856072;

  for (channel = 0; channel < 3; channel++) {
    f64 m1 = m[channel][0];
    f64 m2 = m[channel][1];
    f64 m3 = m[channel][2];

    for (t = 0; t < 2; t++) {
      f64 top1   = (284517.0 * m1 - 94839.0 * m3) * sub2;
      f64 top2   = (838422.0 * m3 + 769860.0 * m2 + 731718.0 * m1) * l * sub2 - 769860.0 * t * l;
      f64 bottom = (632260.0 * m3 - 126452.0 * m2) * sub2 + 126452.0 * t;

      bounds[channel * 2 + t].a = top1 / bottom;
      bounds[channel * 2 + t].b = top2 / bottom;
    }
  }
}

f64 intersect_line_line(const Bounds* line1, const Bounds* line2) {
  return (line1->b - line2->b) / (line2->a - line1->a);
}

f64 dist_from_pole(f64 x, f64 y) {
  return sqrt(x * x + y * y);
}

f64 ray_length_until_intersect(f64 theta, const Bounds* line) {
  return line->b / (sin(theta) - line->a * cos(theta));
}

f64 max_safe_chroma_for_l(f64 l) {
  f64    min_len = FLT_MAX;
  Bounds bounds[6];
  int    i;

  get_bounds(l, bounds);
  for (i = 0; i < 6; i++) {
    f64 m1 = bounds[i].a;
    f64 b1 = bounds[i].b;

    /* x where line intersects with perpendicular running though (0, 0) */
    Bounds line2;
    line2.a = -1.0 / m1;
    line2.b = 0.0;

    f64 x        = intersect_line_line(&bounds[i], &line2);
    f64 distance = dist_from_pole(x, b1 + x * m1);

    if (distance >= 0.0 && distance < min_len)
      min_len = distance;
  }

  return min_len;
}

f64 max_chroma_for_lh(f64 l, f64 h) {
  f64    min_len = FLT_MAX;
  f64    hrad    = h * 0.01745329251994329577; /* (2 * pi / 260) */
  Bounds bounds[6];
  int    i;

  get_bounds(l, bounds);
  for (i = 0; i < 6; i++) {
    f64 l2 = ray_length_until_intersect(hrad, &bounds[i]);

    if (l2 >= 0.0 && l2 < min_len)
      min_len = l2;
  }
  return min_len;
}

/* http://en.wikipedia.org/wiki/CIELUV
 * In these formulas, Yn refers to the reference white point. We are using
 * illuminant D65, so Yn (see refY in Maxima file) equals 1. The formula is
 * simplified accordingly.
 */
f64 y2l(f64 y) {
  if (y <= CIE_EPSILON)
    return y * CIE_KAPPA;
  else
    return 116.0 * cbrt(y) - 16.0;
}

f64 l2y(f64 l) {
  if (l <= 8.0) {
    return l / CIE_KAPPA;
  } else {
    f64 x = (l + 16.0) / 116.0;
    return (x * x * x);
  }
}

Colour64* luv_from_xyz(Colour64* col) {
  f64 var_u =
      (4.0 * col->element[0]) / (col->element[0] + (15.0 * col->element[1]) + (3.0 * col->element[2]));
  f64 var_v =
      (9.0 * col->element[1]) / (col->element[0] + (15.0 * col->element[1]) + (3.0 * col->element[2]));
  f64 l = y2l(col->element[1]);
  f64 u = 13.0 * l * (var_u - ref_u);
  f64 v = 13.0 * l * (var_v - ref_v);

  // SEN_LOG("");
  // SEN_LOG("var_u - ref_u %.5f", (var_u - ref_u));
  // SEN_LOG("var_v - ref_v %.5f", (var_v - ref_v));
  // SEN_LOG("var_u %.5f, var_v %.5f", var_u, var_v);
  // SEN_LOG("");

  col->element[0] = l;
  if (l < 0.00000001) {
    col->element[1] = 0.0;
    col->element[2] = 0.0;
  } else {
    col->element[1] = u;
    col->element[2] = v;
  }

  return col;
}

Colour64* xyz_from_luv(Colour64* col) {
  if (col->element[0] <= 0.00000001) {
    col->element[0] = 0.0;
    col->element[1] = 0.0;
    col->element[2] = 0.0;
    return col;
  }

  f64 var_u = col->element[1] / (13.0 * col->element[0]) + ref_u;
  f64 var_v = col->element[2] / (13.0 * col->element[0]) + ref_v;
  f64 y     = l2y(col->element[0]);
  f64 x     = -(9.0 * y * var_u) / ((var_u - 4.0) * var_v - var_u * var_v);
  f64 z     = (9.0 * y - (15.0 * var_v * y) - (var_v * x)) / (3.0 * var_v);

  col->element[0] = x;
  col->element[1] = y;
  col->element[2] = z;

  return col;
}

Colour64* lch_from_luv(Colour64* col) {
  f64 l = col->element[0];
  f64 u = col->element[1];
  f64 v = col->element[2];
  f64 h;
  f64 c = sqrtf((float)(u * u + v * v));

  // SEN_LOG("lch_from_luv c %.5f", c);

  if (c < 0.00000001f) {
    h = 0.0f;
  } else {
    // SEN_LOG("lch_from_luv atan2f(v, u): %.5f", atan2f(v, u));
    h = atan2f((float)v, (float)u) * 57.29577951308232087680f; /* (180 / pi) */
    if (h < 0.0f)
      h += 360.0f;
  }

  col->element[0] = l;
  col->element[1] = c;
  col->element[2] = h;

  return col;
}

Colour64* luv_from_lch(Colour64* col) {
  f64 hrad = col->element[2] * 0.01745329251994329577f; /* (pi / 180.0) */
  f64 u    = cosf((float)hrad) * col->element[1];
  f64 v    = sinf((float)hrad) * col->element[1];

  col->element[1] = u;
  col->element[2] = v;

  return col;
}

Colour64* lch_from_hsluv(Colour64* col) {
  f64 h = col->element[0];
  f64 s = col->element[1];
  f64 l = col->element[2];
  f64 c;

  if (l > 99.9999999 || l < 0.00000001)
    c = 0.0;
  else
    c = max_chroma_for_lh(l, h) / 100.0 * s;

  if (s < 0.00000001)
    h = 0.0;

  col->element[0] = l;
  col->element[1] = c;
  col->element[2] = h;

  return col;
}

Colour64* hsluv_from_lch(Colour64* col) {
  f64 l = col->element[0];
  f64 c = col->element[1];
  f64 h = col->element[2];
  f64 s;

  if (l > 99.9999999 || l < 0.00000001)
    s = 0.0;
  else
    s = c / max_chroma_for_lh(l, h) * 100.0;

  if (c < 0.00000001)
    h = 0.0;

  col->format     = ColourFormat_HSLuv;
  col->element[0] = h;
  col->element[1] = s;
  col->element[2] = l;

  return col;
}

Colour64* xyz_from_hsluv(Colour64* hsluv) {
  return xyz_from_luv(luv_from_lch(lch_from_hsluv(hsluv)));
}

Colour64* hsluv_from_xyz(Colour64* xyz) {
  return hsluv_from_lch(lch_from_luv(luv_from_xyz(xyz)));
}

Colour64* linear_from_srgb(Colour64* srgb) {
  srgb->format     = ColourFormat_RGB;
  srgb->element[0] = inverse_srgb_companding(srgb->element[0]);
  srgb->element[1] = inverse_srgb_companding(srgb->element[1]);
  srgb->element[2] = inverse_srgb_companding(srgb->element[2]);

  return srgb;
}

Colour64* srgb_from_linear(Colour64* rgb) {
  rgb->format     = ColourFormat_sRGB;
  rgb->element[0] = srgb_companding(rgb->element[0]);
  rgb->element[1] = srgb_companding(rgb->element[1]);
  rgb->element[2] = srgb_companding(rgb->element[2]);

  return rgb;
}

Colour* colour_clone_as(Colour* out, Colour* in, ColourFormat new_format) {
  if (out != in) {
    colour_clone(out, in);
  }

  if (out->format == new_format) {
    return out;
  }

  Colour64 c64;
  colour64_from_colour(&c64, out);

  switch (c64.format) {
  case ColourFormat_HSL:
    switch (new_format) {
    case ColourFormat_HSLuv:
      hsluv_from_xyz(xyz_from_rgb(rgb_from_hsl(&c64)));
      break;
    case ColourFormat_HSV:
      hsv_from_rgb(rgb_from_hsl(&c64));
      break;
    case ColourFormat_LAB:
      lab_from_xyz(xyz_from_rgb(rgb_from_hsl(&c64)));
      break;
    case ColourFormat_RGB:
      rgb_from_hsl(&c64);
      break;
    case ColourFormat_sRGB:
      srgb_from_linear(rgb_from_hsl(&c64));
      break;
    case ColourFormat_OKLAB:
      oklab_from_xyz(xyz_from_rgb(rgb_from_hsl(&c64)));
      break;
    default:
      rona_error("unknown colour format %d", new_format);
      break;
    }
    break;
  case ColourFormat_HSLuv:
    switch (new_format) {
    case ColourFormat_HSL:
      hsl_from_rgb(rgb_from_xyz(xyz_from_hsluv(&c64)));
      break;
    case ColourFormat_HSV:
      hsv_from_rgb(rgb_from_xyz(xyz_from_hsluv(&c64)));
      break;
    case ColourFormat_LAB:
      lab_from_xyz(xyz_from_hsluv(&c64));
      break;
    case ColourFormat_RGB:
      rgb_from_xyz(xyz_from_hsluv(&c64));
      break;
    case ColourFormat_sRGB:
      srgb_from_linear(rgb_from_xyz(xyz_from_hsluv(&c64)));
      break;
    case ColourFormat_OKLAB:
      oklab_from_xyz(xyz_from_hsluv(&c64));
      break;
    default:
      rona_error("unknown colour format %d", new_format);
      break;
    }
    break;
  case ColourFormat_HSV:
    switch (new_format) {
    case ColourFormat_HSL:
      hsl_from_rgb(rgb_from_hsv(&c64));
      break;
    case ColourFormat_HSLuv:
      hsluv_from_xyz(xyz_from_rgb(rgb_from_hsv(&c64)));
      break;
    case ColourFormat_LAB:
      lab_from_xyz(xyz_from_rgb(rgb_from_hsv(&c64)));
      break;
    case ColourFormat_RGB:
      rgb_from_hsv(&c64);
      break;
    case ColourFormat_sRGB:
      srgb_from_linear(rgb_from_hsv(&c64));
      break;
    case ColourFormat_OKLAB:
      oklab_from_xyz(xyz_from_rgb(rgb_from_hsv(&c64)));
      break;
    default:
      rona_error("unknown colour format %d", new_format);
      break;
    }
    break;
  case ColourFormat_LAB:
    switch (new_format) {
    case ColourFormat_HSL:
      hsl_from_rgb(rgb_from_xyz(xyz_from_lab(&c64)));
      break;
    case ColourFormat_HSLuv:
      hsluv_from_xyz(xyz_from_lab(&c64));
      break;
    case ColourFormat_HSV:
      hsv_from_rgb(rgb_from_xyz(xyz_from_lab(&c64)));
      break;
    case ColourFormat_RGB:
      rgb_from_xyz(xyz_from_lab(&c64));
      break;
    case ColourFormat_sRGB:
      srgb_from_linear(rgb_from_xyz(xyz_from_lab(&c64)));
      break;
    case ColourFormat_OKLAB:
      oklab_from_xyz(xyz_from_lab(&c64));
      break;
    default:
      rona_error("unknown colour format %d", new_format);
      break;
    }
    break;
  case ColourFormat_RGB:
    switch (new_format) {
    case ColourFormat_HSL:
      hsl_from_rgb(&c64);
      break;
    case ColourFormat_HSLuv:
      hsluv_from_xyz(xyz_from_rgb(&c64));
      break;
    case ColourFormat_HSV:
      hsv_from_rgb(&c64);
      break;
    case ColourFormat_LAB:
      lab_from_xyz(xyz_from_rgb(&c64));
      break;
    case ColourFormat_sRGB:
      srgb_from_linear(&c64);
      break;
    case ColourFormat_OKLAB:
      // make sure these two lines are equivalent
      // oklab_from_xyz(xyz_from_rgb(&c64)),
      oklab_from_rgb(&c64);
      break;
    default:
      rona_error("unknown colour format %d", new_format);
      break;
    }
    break;
  case ColourFormat_sRGB:
    switch (new_format) {
    case ColourFormat_HSL:
      hsl_from_rgb(linear_from_srgb(&c64));
      break;
    case ColourFormat_HSLuv:
      hsluv_from_xyz(xyz_from_rgb(linear_from_srgb(&c64)));
      break;
    case ColourFormat_HSV:
      hsv_from_rgb(linear_from_srgb(&c64));
      break;
    case ColourFormat_LAB:
      lab_from_xyz(xyz_from_rgb(linear_from_srgb(&c64)));
      break;
    case ColourFormat_RGB:
      linear_from_srgb(&c64);
      break;
    case ColourFormat_OKLAB:
      // make sure these two lines are equivalent
      // oklab_from_xyz(xyz_from_rgb(linear_from_srgb(&c64))),
      oklab_from_rgb(linear_from_srgb(&c64));
      break;
    default:
      rona_error("unknown colour format %d", new_format);
      break;
    }
    break;
  case ColourFormat_OKLAB:
    switch (new_format) {
    case ColourFormat_HSL:
      hsl_from_rgb(rgb_from_xyz(xyz_from_oklab(&c64)));
      break;
    case ColourFormat_HSLuv:
      hsluv_from_xyz(xyz_from_oklab(&c64));
      break;
    case ColourFormat_HSV:
      hsv_from_rgb(rgb_from_xyz(xyz_from_oklab(&c64)));
      break;
    case ColourFormat_LAB:
      oklab_from_xyz(xyz_from_oklab(&c64));
      break;
    case ColourFormat_RGB:
      // rgb_from_xyz(xyz_from_oklab(&c64));
      rgb_from_oklab(&c64); // https://bottosson.github.io/posts/oklab/
      break;
    case ColourFormat_sRGB:
      srgb_from_linear(rgb_from_oklab(&c64));
      break;
    default:
      rona_error("unknown colour format %d", new_format);
      break;
    }
    break;
  default:
    rona_error("unknown colour format %d", in->format);
    break;
  }

  colour_from_colour64(out, &c64);

  return out;
}

void vec4_from_colour(Vec4* out, Colour* in) {
  out->r = in->element[0];
  out->g = in->element[1];
  out->b = in->element[2];
  out->a = in->element[3];
}
