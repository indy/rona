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


// void vec3_set(Vec3* v, f32 x, f32 y, f32 z);

void mat4_identity(Mat4* m) {
  m->v[0] = 1.0f;
  m->v[1] = 0.0f;
  m->v[2] = 0.0f;
  m->v[3] = 0.0f;
  m->v[4] = 0.0f;
  m->v[5] = 1.0f;
  m->v[6] = 0.0f;
  m->v[7] = 0.0f;
  m->v[8] = 0.0f;
  m->v[9] = 0.0f;
  m->v[10] = 1.0f;
  m->v[11] = 0.0f;
  m->v[12] = 0.0f;
  m->v[13] = 0.0f;
  m->v[14] = 0.0f;
  m->v[15] = 1.0f;
}

void mat4_ortho(Mat4* m, f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {
  f32 lr = 1.0f / (left - right);
  f32 bt = 1.0f / (bottom - top);
  f32 nf = 1.0f / (near - far);

  m->v[0] = -2.0f * lr;
  m->v[1] = 0.0f;
  m->v[2] = 0.0f;
  m->v[3] = 0.0f;
  m->v[4] = 0.0f;
  m->v[5] = -2.0f * bt;
  m->v[6] = 0.0f;
  m->v[7] = 0.0f;
  m->v[8] = 0.0f;
  m->v[9] = 0.0f;
  m->v[10] = 2.0f * nf;
  m->v[11] = 0.0f;
  m->v[12] = (left + right) * lr;
  m->v[13] = (top + bottom) * bt;
  m->v[14] = (far + near) * nf;
  m->v[15] = 1.0f;
}

void mat4_copy(Mat4* a, Mat4* b) {
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

void mat4_multiply(Mat4* out, Mat4* a, Mat4* b) {
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

    out->v[0] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
    out->v[1] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
    out->v[2] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
    out->v[3] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;
  }

  {
    f32 b0 = b->v[4];
    f32 b1 = b->v[5];
    f32 b2 = b->v[6];
    f32 b3 = b->v[7];
    out->v[4] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
    out->v[5] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
    out->v[6] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
    out->v[7] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;
  }
  {
    f32 b0 = b->v[8];
    f32 b1 = b->v[9];
    f32 b2 = b->v[10];
    f32 b3 = b->v[11];
    out->v[8] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
    out->v[9] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
    out->v[10] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
    out->v[11] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;
  }
  {
    f32 b0 = b->v[12];
    f32 b1 = b->v[13];
    f32 b2 = b->v[14];
    f32 b3 = b->v[15];
    out->v[12] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
    out->v[13] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
    out->v[14] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
    out->v[15] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;
  }
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
