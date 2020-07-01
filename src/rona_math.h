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

#ifndef RONA_MATH_H
#define RONA_MATH_H

#define SINF sinf
#define COSF cosf
#define TANF tanf
#define SQRTF sqrtf
#define EXPF expf
#define LOGF logf
#define ACOSF acosf
#define ATANF atanf
#define ATAN2F atan2f

#define PI32 3.14159265359f
#define PI 3.14159265358979323846
#define TAU32 (PI32 + PI32)
#define TAU (PI + PI)

#define MIN(a, b) (a) > (b) ? (b) : (a)
#define MAX(a, b) (a) < (b) ? (b) : (a)
#define ABS(a) ((a) > 0 ? (a) : -(a))
#define MOD(a, m) ((a) % (m)) >= 0 ? ((a) % (m)) : (((a) % (m)) + (m))
#define SQUARE(x) ((x) * (x))

f32 to_radians(f32 degrees);
f32 lerp(f32 a, f32 t, f32 b);
f32 clamp(f32 min, f32 val, f32 max);

Vec2i vec2i(i32 x, i32 y);
Vec2i vec2i_clone(Vec2i *v2i);
void  vec2i_set(Vec2i *dest, i32 x, i32 y);
Vec2i vec2i_add(Vec2i a, Vec2i b);
Vec2i vec2i_sub(Vec2i a, Vec2i b);
Vec2i vec2i_mul(Vec2i a, Vec2i b);
Vec2i vec2i_div(Vec2i a, Vec2i b);
i32   vec2i_dot(Vec2i a, Vec2i b);
i32   vec2i_length_squared(Vec2i a);

Vec2 vec2(f32 x, f32 y);
Vec2 vec2_clone(Vec2 *v2);
void vec2_set(Vec2 *dest, f32 x, f32 y);
Vec2 vec2_add(Vec2 a, Vec2 b);
Vec2 vec2_sub(Vec2 a, Vec2 b);
Vec2 vec2_mul(Vec2 a, Vec2 b);
Vec2 vec2_div(Vec2 a, Vec2 b);
f32  vec2_dot(Vec2 a, Vec2 b);
f32  vec2_length_squared(Vec2 a);
f32  vec2_length(Vec2 a);
Vec2 vec2_normalize(Vec2 a);

Vec3 vec3(f32 x, f32 y, f32 z);
Vec3 vec3_clone(Vec3 *v3);
void vec3_set(Vec3 *dest, f32 x, f32 y, f32 z);
Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3_sub(Vec3 a, Vec3 b);
Vec3 vec3_mul(Vec3 a, Vec3 b);
Vec3 vec3_div(Vec3 a, Vec3 b);
f32  vec3_dot(Vec3 a, Vec3 b);
Vec3 vec3_cross(Vec3 a, Vec3 b);
f32  vec3_length_squared(Vec3 a);
f32  vec3_length(Vec3 a);
Vec3 vec3_normalize(Vec3 a);
Vec3 vec3_normalize_np(Vec3 a);

Vec4 vec4(f32 x, f32 y, f32 z, f32 w);
Vec4 vec4_clone(Vec4 *v4);
void vec4_set(Vec4 *dest, f32 x, f32 y, f32 z, f32 w);
Vec4 vec4_add(Vec4 a, Vec4 b);
Vec4 vec4_sub(Vec4 a, Vec4 b);
Vec4 vec4_mul(Vec4 a, Vec4 b);
Vec4 vec4_div(Vec4 a, Vec4 b);
f32  vec4_dot(Vec4 a, Vec4 b);
f32  vec4_length_squared(Vec4 a);
f32  vec4_length(Vec4 a);
Vec4 vec4_normalize(Vec4 a);

Mat4 mat4_identity();
Mat4 mat4_ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
Mat4 mat4_perspective(f32 fov, f32 aspect_ratio, f32 near, f32 far);
Mat4 mat4_multiply(Mat4 *a, Mat4 *b);
void mat4_multiply_inplace(Mat4 *a, Mat4 *b);
Mat4 mat4_copy(Mat4 *b);
void mat4_copy_inplace(Mat4 *a, Mat4 *b);
Mat4 mat4_rotate(f32 angle_in_radians, Vec3 axis);
Mat4 mat4_look_at(Vec3 from, Vec3 at, Vec3 up);

#endif /* RONA_MATH_H */
