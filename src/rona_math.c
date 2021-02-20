// Copyright (C) 2020 Inderjit Gill <email@indy.io>

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

f32 to_radians(f32 degrees) {
  return degrees * (TAU32 / 360.0f);
}

f32 lerp(f32 a, f32 t, f32 b) {
  return ((b - a) * t) + a;
}

f32 clamp(f32 min, f32 val, f32 max) {
  return val < min ? min : val > max ? max : val;
}

Dim2 dim2(usize width, usize height) {
  Dim2 res;

  res.width = width;
  res.height = height;

  return res;
}

Rect rect(i32 x, i32 y, usize width, usize height) {
  Rect r;

  r.pos.x = x;
  r.pos.y = y;
  r.dim.width = width;
  r.dim.height = height;

  return r;
}

bool rect_contains_point(Rect rect, Vec2i point) {
  return point.x >= rect.pos.x && point.x < (rect.pos.x + rect.dim.width) &&
         point.y >= rect.pos.y && point.y < (rect.pos.y + rect.dim.height);
}

Vec2i vec2i(i32 x, i32 y) {
  Vec2i res;

  res.x = x;
  res.y = y;

  return res;
}

Vec2i vec2i_clone(Vec2i* v2i) {
  Vec2i res;

  res.x = v2i->x;
  res.y = v2i->y;

  return res;
}

void vec2i_set(Vec2i* dest, i32 x, i32 y) {
  dest->x = x;
  dest->y = y;
}

void vec2i_copy(Vec2i* dest, Vec2i* src) {
  dest->x = src->x;
  dest->y = src->y;
}

Vec2i vec2i_add(Vec2i a, Vec2i b) {
  Vec2i res;
  res.x = a.x + b.x;
  res.y = a.y + b.y;
  return res;
}

Vec2i vec2i_sub(Vec2i a, Vec2i b) {
  Vec2i res;
  res.x = a.x - b.x;
  res.y = a.y - b.y;
  return res;
}

Vec2i vec2i_mul(Vec2i a, Vec2i b) {
  Vec2i res;
  res.x = a.x * b.x;
  res.y = a.y * b.y;
  return res;
}

Vec2i vec2i_div(Vec2i a, Vec2i b) {
  Vec2i res;
  res.x = a.x / b.x;
  res.y = a.y / b.y;
  return res;
}

Vec2i vec2i_mod(Vec2i a, Vec2i b) {
  Vec2i res;
  res.x = a.x % b.x;
  res.y = a.y % b.y;
  return res;
}

bool vec2i_eq(Vec2i a, Vec2i b) {
  return a.x == b.x && a.y == b.y;
}

i32 vec2i_dot(Vec2i a, Vec2i b) {
  i32 res;
  res = (a.x * b.x) + (a.y * b.y);
  return res;
}

i32 vec2i_length_squared(Vec2i a) {
  return vec2i_dot(a, a);
}

void vec2i_log(char* msg, Vec2i a) {
  RONA_LOG("%s: (%d,%d)\n", msg, a.x, a.y);
}

Vec2 vec2(f32 x, f32 y) {
  Vec2 res;

  res.x = x;
  res.y = y;

  return res;
}

Vec2 vec2_clone(Vec2* v2) {
  Vec2 res;

  res.x = v2->x;
  res.y = v2->y;

  return res;
}

void vec2_set(Vec2* dest, f32 x, f32 y) {
  dest->x = x;
  dest->y = y;
}

Vec2 vec2_add(Vec2 a, Vec2 b) {
  Vec2 res;
  res.x = a.x + b.x;
  res.y = a.y + b.y;
  return res;
}

Vec2 vec2_sub(Vec2 a, Vec2 b) {
  Vec2 res;
  res.x = a.x - b.x;
  res.y = a.y - b.y;
  return res;
}

Vec2 vec2_mul(Vec2 a, Vec2 b) {
  Vec2 res;
  res.x = a.x * b.x;
  res.y = a.y * b.y;
  return res;
}

Vec2 vec2_div(Vec2 a, Vec2 b) {
  Vec2 res;
  res.x = a.x / b.x;
  res.y = a.y / b.y;
  return res;
}

f32 vec2_dot(Vec2 a, Vec2 b) {
  f32 res;
  res = (a.x * b.x) + (a.y * b.y);
  return res;
}

f32 vec2_length_squared(Vec2 a) {
  return vec2_dot(a, a);
}

f32 vec2_length(Vec2 a) {
  return SQRTF(vec2_length_squared(a));
}

Vec2 vec2_normalize(Vec2 a) {
  Vec2 res = vec2(0.0f, 0.0f);
  f32  len = vec2_length(a);
  if (len != 0.0f) {
    res.x = a.x / len;
    res.y = a.y / len;
  }
  return res;
}

Vec3 vec3(f32 x, f32 y, f32 z) {
  Vec3 res;

  res.x = x;
  res.y = y;
  res.z = z;

  return res;
}

Vec3 vec3_clone(Vec3* v3) {
  Vec3 res;

  res.x = v3->x;
  res.y = v3->y;
  res.z = v3->z;

  return res;
}

void vec3_set(Vec3* dest, f32 x, f32 y, f32 z) {
  dest->x = x;
  dest->y = y;
  dest->z = z;
}

void vec3_copy(Vec3* dest, Vec3* src) {
  dest->x = src->x;
  dest->y = src->y;
  dest->z = src->z;
}

Vec3 vec3_add(Vec3 a, Vec3 b) {
  Vec3 res;
  res.x = a.x + b.x;
  res.y = a.y + b.y;
  res.z = a.z + b.z;
  return res;
}

Vec3 vec3_sub(Vec3 a, Vec3 b) {
  Vec3 res;
  res.x = a.x - b.x;
  res.y = a.y - b.y;
  res.z = a.z - b.z;
  return res;
}

Vec3 vec3_mul(Vec3 a, Vec3 b) {
  Vec3 res;
  res.x = a.x * b.x;
  res.y = a.y * b.y;
  res.z = a.z * b.z;
  return res;
}

Vec3 vec3_div(Vec3 a, Vec3 b) {
  Vec3 res;
  res.x = a.x / b.x;
  res.y = a.y / b.y;
  res.z = a.z / b.z;
  return res;
}

f32 vec3_dot(Vec3 a, Vec3 b) {
  f32 res;
  res = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
  return res;
}

Vec3 vec3_cross(Vec3 a, Vec3 b) {
  Vec3 res;
  res.x = (a.y * b.z) - (a.z * b.y);
  res.y = (a.z * b.x) - (a.x * b.z);
  res.z = (a.x * b.y) - (a.y * b.x);
  return res;
}

f32 vec3_length_squared(Vec3 a) {
  return vec3_dot(a, a);
}

f32 vec3_length(Vec3 a) {
  return SQRTF(vec3_length_squared(a));
}

Vec3 vec3_normalize(Vec3 a) {
  Vec3 res = vec3(0.0f, 0.0f, 0.0f);
  f32  len = vec3_length(a);
  if (len != 0.0f) {
    res.x = a.x / len;
    res.y = a.y / len;
    res.z = a.z / len;
  }
  return res;
}

Vec4 vec4(f32 x, f32 y, f32 z, f32 w) {
  Vec4 res;

  res.x = x;
  res.y = y;
  res.z = z;
  res.w = w;

  return res;
}

Vec4 vec4_clone(Vec4* v4) {
  Vec4 res;

  res.x = v4->x;
  res.y = v4->y;
  res.z = v4->z;
  res.w = v4->w;

  return res;
}

void vec4_set(Vec4* dest, f32 x, f32 y, f32 z, f32 w) {
  dest->x = x;
  dest->y = y;
  dest->z = z;
  dest->w = w;
}

Vec4 vec4_add(Vec4 a, Vec4 b) {
  Vec4 res;
  res.x = a.x + b.x;
  res.y = a.y + b.y;
  res.z = a.z + b.z;
  res.w = a.w + b.w;
  return res;
}

Vec4 vec4_sub(Vec4 a, Vec4 b) {
  Vec4 res;
  res.x = a.x - b.x;
  res.y = a.y - b.y;
  res.z = a.z - b.z;
  res.w = a.w - b.w;
  return res;
}

Vec4 vec4_mul(Vec4 a, Vec4 b) {
  Vec4 res;
  res.x = a.x * b.x;
  res.y = a.y * b.y;
  res.z = a.z * b.z;
  res.w = a.w * b.w;
  return res;
}

Vec4 vec4_div(Vec4 a, Vec4 b) {
  Vec4 res;
  res.x = a.x / b.x;
  res.y = a.y / b.y;
  res.z = a.z / b.z;
  res.w = a.w / b.w;
  return res;
}

f32 vec4_dot(Vec4 a, Vec4 b) {
  f32 res;
  res = (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
  return res;
}

f32 vec4_length_squared(Vec4 a) {
  return vec4_dot(a, a);
}

f32 vec4_length(Vec4 a) {
  return SQRTF(vec4_length_squared(a));
}

Vec4 vec4_normalize(Vec4 a) {
  Vec4 res = vec4(0.0f, 0.0f, 0.0f, 0.0f);
  f32  len = vec4_length(a);
  if (len != 0.0f) {
    res.x = a.x / len;
    res.y = a.y / len;
    res.z = a.z / len;
    res.w = a.w / len;
  }
  return res;
}

Mat4 mat4_identity() {
  Mat4 m;
  m.v[0] = 1.0f;
  m.v[1] = 0.0f;
  m.v[2] = 0.0f;
  m.v[3] = 0.0f;
  m.v[4] = 0.0f;
  m.v[5] = 1.0f;
  m.v[6] = 0.0f;
  m.v[7] = 0.0f;
  m.v[8] = 0.0f;
  m.v[9] = 0.0f;
  m.v[10] = 1.0f;
  m.v[11] = 0.0f;
  m.v[12] = 0.0f;
  m.v[13] = 0.0f;
  m.v[14] = 0.0f;
  m.v[15] = 1.0f;
  return m;
}

Mat4 mat4_ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {
  Mat4 m;
  f32  lr = 1.0f / (left - right);
  f32  bt = 1.0f / (bottom - top);
  f32  nf = 1.0f / (near - far);

  m.v[0] = -2.0f * lr;
  m.v[1] = 0.0f;
  m.v[2] = 0.0f;
  m.v[3] = 0.0f;
  m.v[4] = 0.0f;
  m.v[5] = -2.0f * bt;
  m.v[6] = 0.0f;
  m.v[7] = 0.0f;
  m.v[8] = 0.0f;
  m.v[9] = 0.0f;
  m.v[10] = 2.0f * nf;
  m.v[11] = 0.0f;
  m.v[12] = (left + right) * lr;
  m.v[13] = (top + bottom) * bt;
  m.v[14] = (far + near) * nf;
  m.v[15] = 1.0f;

  return m;
}

Mat4 mat4_perspective(f32 fov, f32 aspect_ratio, f32 near, f32 far) {
  Mat4  m;
  float cotangent = 1.0f / TANF(fov * (PI32 / 360.0f));

  m.v[0] = cotangent * aspect_ratio;
  m.v[1] = 0.0f;
  m.v[2] = 0.0f;
  m.v[3] = 0.0f;
  m.v[4] = 0.0f;
  m.v[5] = cotangent;
  m.v[6] = 0.0f;
  m.v[7] = 0.0f;
  m.v[8] = 0.0f;
  m.v[9] = 0.0f;
  m.v[10] = (near + far) / (near - far);
  m.v[11] = -1.0f;
  m.v[12] = 0.0f;
  m.v[13] = 0.0f;
  m.v[14] = (2.0f * near * far) / (near - far);
  m.v[15] = 0.0f;

  return m;
}

Mat4 mat4_multiply(Mat4* a, Mat4* b) {
  Mat4 out;

  f32 a00 = a->v[0];
  f32 a01 = a->v[1];
  f32 a02 = a->v[2];
  f32 a03 = a->v[3];
  f32 a10 = a->v[4];
  f32 a11 = a->v[5];
  f32 a12 = a->v[6];
  f32 a13 = a->v[7];
  f32 a20 = a->v[8];
  f32 a21 = a->v[9];
  f32 a22 = a->v[10];
  f32 a23 = a->v[11];
  f32 a30 = a->v[12];
  f32 a31 = a->v[13];
  f32 a32 = a->v[14];
  f32 a33 = a->v[15];

  {
    f32 b0 = b->v[0];
    f32 b1 = b->v[1];
    f32 b2 = b->v[2];
    f32 b3 = b->v[3];

    out.v[0] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
    out.v[1] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
    out.v[2] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
    out.v[3] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;
  }

  {
    f32 b0 = b->v[4];
    f32 b1 = b->v[5];
    f32 b2 = b->v[6];
    f32 b3 = b->v[7];
    out.v[4] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
    out.v[5] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
    out.v[6] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
    out.v[7] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;
  }
  {
    f32 b0 = b->v[8];
    f32 b1 = b->v[9];
    f32 b2 = b->v[10];
    f32 b3 = b->v[11];
    out.v[8] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
    out.v[9] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
    out.v[10] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
    out.v[11] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;
  }
  {
    f32 b0 = b->v[12];
    f32 b1 = b->v[13];
    f32 b2 = b->v[14];
    f32 b3 = b->v[15];
    out.v[12] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
    out.v[13] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
    out.v[14] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
    out.v[15] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;
  }

  return out;
}

// a = a * b
void mat4_multiply_inplace(Mat4* a, Mat4* b) {
  f32 a00 = a->v[0];
  f32 a01 = a->v[1];
  f32 a02 = a->v[2];
  f32 a03 = a->v[3];
  f32 a10 = a->v[4];
  f32 a11 = a->v[5];
  f32 a12 = a->v[6];
  f32 a13 = a->v[7];
  f32 a20 = a->v[8];
  f32 a21 = a->v[9];
  f32 a22 = a->v[10];
  f32 a23 = a->v[11];
  f32 a30 = a->v[12];
  f32 a31 = a->v[13];
  f32 a32 = a->v[14];
  f32 a33 = a->v[15];

  {
    f32 b0 = b->v[0];
    f32 b1 = b->v[1];
    f32 b2 = b->v[2];
    f32 b3 = b->v[3];

    a->v[0] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
    a->v[1] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
    a->v[2] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
    a->v[3] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;
  }

  {
    f32 b0 = b->v[4];
    f32 b1 = b->v[5];
    f32 b2 = b->v[6];
    f32 b3 = b->v[7];
    a->v[4] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
    a->v[5] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
    a->v[6] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
    a->v[7] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;
  }
  {
    f32 b0 = b->v[8];
    f32 b1 = b->v[9];
    f32 b2 = b->v[10];
    f32 b3 = b->v[11];
    a->v[8] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
    a->v[9] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
    a->v[10] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
    a->v[11] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;
  }
  {
    f32 b0 = b->v[12];
    f32 b1 = b->v[13];
    f32 b2 = b->v[14];
    f32 b3 = b->v[15];
    a->v[12] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
    a->v[13] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
    a->v[14] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
    a->v[15] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;
  }
}

Mat4 mat4_copy(Mat4* b) {
  Mat4 a;
  a.v[0] = b->v[0];
  a.v[1] = b->v[1];
  a.v[2] = b->v[2];
  a.v[3] = b->v[3];
  a.v[4] = b->v[4];
  a.v[5] = b->v[5];
  a.v[6] = b->v[6];
  a.v[7] = b->v[7];
  a.v[8] = b->v[8];
  a.v[9] = b->v[9];
  a.v[10] = b->v[10];
  a.v[11] = b->v[11];
  a.v[12] = b->v[12];
  a.v[13] = b->v[13];
  a.v[14] = b->v[14];
  a.v[15] = b->v[15];

  return a;
}

void mat4_copy_inplace(Mat4* a, Mat4* b) {
  a->v[0] = b->v[0];
  a->v[1] = b->v[1];
  a->v[2] = b->v[2];
  a->v[3] = b->v[3];
  a->v[4] = b->v[4];
  a->v[5] = b->v[5];
  a->v[6] = b->v[6];
  a->v[7] = b->v[7];
  a->v[8] = b->v[8];
  a->v[9] = b->v[9];
  a->v[10] = b->v[10];
  a->v[11] = b->v[11];
  a->v[12] = b->v[12];
  a->v[13] = b->v[13];
  a->v[14] = b->v[14];
  a->v[15] = b->v[15];
}

Mat4 mat4_rotate(f32 angle_in_radians, Vec3 axis) {
  Mat4 m = mat4_identity();

  Vec3 a = vec3_normalize(axis);

  f32 sin_theta = SINF(angle_in_radians);
  f32 cos_theta = COSF(angle_in_radians);
  f32 cos_value = 1.0f - cos_theta;

  m.m11 = (a.x * a.x * cos_value) + cos_theta;
  m.m12 = (a.x * a.y * cos_value) + (a.z * sin_theta);
  m.m13 = (a.x * a.z * cos_value) - (a.y * sin_theta);

  m.m21 = (a.y * a.x * cos_value) - (a.z * sin_theta);
  m.m22 = (a.y * a.y * cos_value) + cos_theta;
  m.m23 = (a.y * a.z * cos_value) + (a.x * sin_theta);

  m.m31 = (a.z * a.x * cos_value) + (a.y * sin_theta);
  m.m32 = (a.z * a.y * cos_value) - (a.x * sin_theta);
  m.m33 = (a.z * a.z * cos_value) + cos_theta;

  return m;
}

Mat4 mat4_look_at(Vec3 from, Vec3 at, Vec3 up) {
  Mat4 m;

  Vec3 f = vec3_normalize(vec3_sub(at, from));
  Vec3 s = vec3_normalize(vec3_cross(f, up));
  Vec3 u = vec3_cross(s, f);

  m.m11 = s.x;
  m.m12 = u.x;
  m.m13 = -f.x;
  m.m14 = 0.0f;

  m.m21 = s.y;
  m.m22 = u.y;
  m.m23 = -f.y;
  m.m24 = 0.0f;

  m.m31 = s.z;
  m.m32 = u.z;
  m.m33 = -f.z;
  m.m34 = 0.0f;

  m.m41 = -vec3_dot(s, from);
  m.m42 = -vec3_dot(u, from);
  m.m43 = vec3_dot(f, from);
  m.m44 = 1.0f;

  return m;
}
