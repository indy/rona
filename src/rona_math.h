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

// void vec3_set(Vec3* v, f32 x, f32 y, f32 z);

void mat4_identity(Mat4* m);
void mat4_ortho(Mat4* m, f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
void mat4_multiply_inplace(Mat4* a, Mat4* b);
void mat4_multiply(Mat4* out, Mat4* a, Mat4* b);
void mat4_copy(Mat4* a, Mat4* b);

#endif /* RONA_MATH_H */
