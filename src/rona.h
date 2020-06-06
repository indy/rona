#ifndef RONA_H
#define RONA_H

#define true	1
#define false	0

typedef _Bool bool;
typedef float f32;
typedef double f64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short i16;
typedef unsigned int u32;
typedef signed int i32;
typedef unsigned long long u64;

typedef struct vec3 {
  union {
    struct {
      f32 x;
      f32 y;
      f32 z;
    };
    f32 v[3];
  };
} vec3;

typedef struct vec4 {
  union {
    struct {
      f32 x;
      f32 y;
      f32 z;
      f32 w;
    };
    f32 v[4];
  };
} vec4;

/*
column major

Matrix 4×4 representation:
0/m11 4/m12  8/m13 12/m14
1/m21 5/m22  9/m23 13/m24
2/m31 6/m32 10/m33 14/m34
3/m41 7/m42 11/m43 15/m44

https://www.opengl.org/archives/resources/faq/technical/transformations.htm

For programming purposes, OpenGL matrices are 16-value arrays with base vectors laid out contiguously in memory. The translation components occupy the 13th, 14th, and 15th elements of the 16-element matrix, where indices are numbered from 1 to 16 as described in section 2.11.2 of the OpenGL 2.1 Specification.

Column-major versus row-major is purely a notational convention. Note that post-multiplying with column-major matrices produces the same result as pre-multiplying with row-major matrices. The OpenGL Specification and the OpenGL Reference Manual both use column-major notation. You can use any notation, as long as it's clearly stated.

Sadly, the use of column-major format in the spec and blue book has resulted in endless confusion in the OpenGL programming community. Column-major notation suggests that matrices are not laid out in memory as a programmer would expect.

unreal is row major
unity is colum major
*/

typedef struct mat4 {
  union {
		struct {
			f32 m11;
      f32 m21;
      f32 m31;
      f32 m41;
			f32 m12;
      f32 m22;
      f32 m32;
      f32 m42;
			f32 m13;
      f32 m23;
      f32 m33;
      f32 m43;
			f32 m14;
      f32 m24;
      f32 m34;
      f32 m44;
		};
		f32 v[16];
	};
} mat4;

void vec3_set(vec3* v, f32 x, f32 y, f32 z);


#endif /* RONA_H */
