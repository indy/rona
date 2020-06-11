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

#ifndef GAME_STATE_H
#define GAME_STATE_H


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

  Vec2 world_pos;
  Vec2 world_target;
  f32 world_max_speed;
} Entity;


typedef enum {
  TileType_Void,
  TileType_Floor
} TileType;

typedef struct {
  MemoryArena mem;

  i32 max_num_entities;
  Entity* entities;

  i32 width;
  i32 height;
  TileType *tiles;
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


#endif /* GAME_STATE_H */
