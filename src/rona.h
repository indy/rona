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

#ifndef RONA_H
#define RONA_H

#define true	1
#define false	0

#ifndef __cplusplus
typedef _Bool bool;
#endif
typedef float f32;
typedef double f64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short i16;
typedef unsigned int u32;
typedef signed int i32;
typedef unsigned long long u64;

typedef struct {
  union {
    struct {
      f32 x;
      f32 y;
    };
    f32 v[2];
  };
} Vec2;

typedef struct {
  union {
    struct {
      i32 x;
      i32 y;
    };
    i32 v[2];
  };
} Vec2i;

typedef struct {
  union {
    struct {
      f32 x;
      f32 y;
      f32 z;
    };
    f32 v[3];
  };
} Vec3;

typedef struct {
  union {
    struct {
      f32 x;
      f32 y;
      f32 z;
      f32 w;
    };
    struct {
      f32 r;
      f32 g;
      f32 b;
      f32 a;
    };
    f32 v[4];
  };
} Vec4;

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

typedef struct Mat4 {
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
} Mat4;

typedef struct {
  void *base;
  u64 size;
  u64 used;
} MemoryArena;


typedef enum {
  MeshType_Hero
} MeshType;

// typedef struct {
//   union {
//     struct {
//     } hero;
//   };
// } MeshUniform;

typedef struct {
  MeshType mesh_type;
  GLuint shader_program;
  GLuint vao;

  // universal uniforms common to all meshes
  int uniform_colour;
  int uniform_proj_matrix;
  int uniform_pos;

  // MeshUniform uniform;                         // discriminated union

  i32 num_elements;             // used in by gl->drawElements
} Mesh;

typedef enum {
  EntityType_Hero,
  EntityType_Block,
  EntityType_Pit
} EntityType;

typedef enum {
  EntityState_Standing,
  EntityState_Moving
} EntityState;

typedef struct Entity {
  EntityType entity_type;
  EntityState entity_state;

  // when looping through entities in a level stop at the first one that doesn't exist
  // there will be no more entites where exists == true
  bool exists;

  Mesh *mesh;
  Vec4 colour;

  Vec2i board_pos;

  Vec3 world_pos;
  Vec3 world_target;
  f32 world_max_speed;
} Entity;

typedef enum {
  Direction_North,
  Direction_South,
  Direction_East,
  Direction_West
} Direction;

typedef enum {
  TileType_Void,
  TileType_Floor
} TileType;

typedef struct {
  TileType tile_type;
} Tile;

typedef struct {
  MemoryArena mem;

  i32 max_num_entities;
  Entity* entities;

  i32 width;
  i32 height;
  Tile *tiles;
  Mesh *mesh_floor;

  // history/replay data goes here as well
} Level;

typedef struct  {
  bool game_initialised;
  bool quit_game;

  bool window_has_focus;
  bool window_resized;
  i32 window_width;
  i32 window_height;

  u64 time_game_start;   // time at the start of the game
  u64 time_last_frame;
  u64 time_delta;

  MemoryArena storage_permanent;
  MemoryArena storage_transient;

  Mesh *mesh_hero;
  Mesh *mesh_block;
  Mesh *mesh_pit;

  Level *level;

  RonaGl *gl;
  RonaInput *input;
} GameState;


#endif /* RONA_H */
