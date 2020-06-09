#ifndef RONA_MATH_H
#define RONA_MATH_H

// void vec3_set(Vec3* v, f32 x, f32 y, f32 z);

void mat4_identity(Mat4* m);
void mat4_ortho(Mat4* m, f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
void mat4_multiply_inplace(Mat4* a, Mat4* b);
void mat4_multiply(Mat4* out, Mat4* a, Mat4* b);
void mat4_copy(Mat4* a, Mat4* b);

#endif /* RONA_MATH_H */
