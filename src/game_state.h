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

  // MeshUniform uniform;                         // discriminated union


  i32 num_elements;             // used in by gl->drawElements
} Mesh;

typedef enum {
  EntityType_Hero
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
} Entity;

typedef struct {
  i32 width;
  i32 height;
  i32 max_num_entities;

  MemoryArena mem;
  Entity* entities;


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

  Level *level;

  RonaGl *gl;
  RonaInput *input;
} GameState;


#endif /* GAME_STATE_H */
