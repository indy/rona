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

#ifndef RONA_H
#define RONA_H

// Configuration
//
// define RONA_EDITOR to compile in nuklear code

#define RONA_EDITOR

// Memory allocation sizes
//

// sum of these 2 is the total reserved for the entire game
#define MEMORY_ALLOCATION_ARENA_PERMANENT 256
#define MEMORY_ALLOCATION_ARENA_TRANSIENT 256

#define MEMORY_ALLOCATION_LEVEL 64
#define MEMORY_ALLOCATION_NUKLEAR 16
#define MEMORY_ALLOCATION_NUKLEAR_ATLAS 16

// Fixed sizes that affect memory on CPU or GPU
#define MEMORY_RESERVE_COMMANDS_IN_BUFFER 100
#define MEMORY_RESERVE_MAX_ENTITIES_TO_RENDER 500

#ifdef _DEBUG
#define RONA_ASSERT(exp)                                                                                     \
  if (!(exp)) {                                                                                              \
    fprintf(stderr, "ASSERT FAILURE: %s (%s:%d)\n", __func__, __FILE__, __LINE__);                           \
    fflush(stderr);                                                                                          \
    *(int*)0 = 0;                                                                                            \
  }
#else
#define RONA_ASSERT(exp)                                                                                     \
  {}
#endif

#define RONA_ERROR(...) fprintf(stderr, ##__VA_ARGS__)
#define RONA_INFO(...) fprintf(stdout, ##__VA_ARGS__)
#define RONA_LOG(...) fprintf(stdout, ##__VA_ARGS__)

#define true 1
#define false 0

#ifndef __cplusplus
typedef _Bool bool;
#endif
typedef float              f32;
typedef double             f64;
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef signed short       i16;
typedef unsigned int       u32;
typedef signed int         i32;
typedef unsigned long long u64;

typedef unsigned char byte;
typedef unsigned int  usize;

#include "rona_gl.h"

typedef struct {
  union {
    struct {
      usize width;
      usize height;
    };
    struct {
      usize col;
      usize row;
    };
    usize e[2];
  };
} Dim2;

typedef struct {
  union {
    struct {
      i32 x;
      i32 y;
    };
    i32 e[2];
  };
} Vec2i;

typedef struct {
  Vec2i pos; // the top-left corner
  Dim2  dim; // width, height
} Rect;

typedef struct {
  union {
    struct {
      f32 x;
      f32 y;
    };
    struct {
      f32 u;
      f32 v;
    };
    struct {
      f32 width;
      f32 height;
    };
    //    f32 e[2];
  };
} Vec2;

typedef struct {
  union {
    struct {
      f32 x;
      f32 y;
      f32 z;
    };
    f32 e[3];
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
    f32 e[4];
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

For programming purposes, OpenGL matrices are 16-value arrays with base vectors laid out
contiguously in memory. The translation components occupy the 13th, 14th, and 15th elements of the
16-element matrix, where indices are numbered from 1 to 16 as described in section 2.11.2 of the
OpenGL 2.1 Specification.

Column-major versus row-major is purely a notational convention. Note that post-multiplying with
column-major matrices produces the same result as pre-multiplying with row-major matrices. The
OpenGL Specification and the OpenGL Reference Manual both use column-major notation. You can use any
notation, as long as it's clearly stated.

Sadly, the use of column-major format in the spec and blue book has resulted in endless confusion in
the OpenGL programming community. Column-major notation suggests that matrices are not laid out in
memory as a programmer would expect.

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

/*
** Tileset Sprites
**
*/

// SPRITE_DECL parameters: ENUM, TILE_X, TILE_Y, OFFSET_X, OFFSET_Y
//
#define FOREACH_SPRITE(SPRITE_DECL)                                                                          \
  SPRITE_DECL(S_Blank, 2, 0, 0.0f, 0.0f)                                                                     \
  SPRITE_DECL(S_Floor01, 1, 25, 0.0f, 0.0f)                                                                  \
  SPRITE_DECL(S_Floor02, 1, 26, 0.0f, 0.0f)                                                                  \
  SPRITE_DECL(S_Floor03, 1, 27, 0.0f, 0.0f)                                                                  \
  SPRITE_DECL(S_Floor04, 1, 28, 0.0f, 0.0f)                                                                  \
  SPRITE_DECL(S_Floor05, 1, 29, 0.0f, 0.0f)                                                                  \
  SPRITE_DECL(S_WallBottom, 2, 25, 0.0f, 0.0f)                                                               \
  SPRITE_DECL(S_WallTop, 2, 26, 0.0f, 0.0f)                                                                  \
  SPRITE_DECL(S_WallLeft, 2, 27, 0.0f, 0.0f)                                                                 \
  SPRITE_DECL(S_WallRight, 2, 28, 0.0f, 0.0f)                                                                \
  SPRITE_DECL(S_CornerA_TL, 2, 29, 0.0f, 0.0f)                                                               \
  SPRITE_DECL(S_CornerA_TR, 2, 30, 0.0f, 0.0f)                                                               \
  SPRITE_DECL(S_CornerA_BL, 2, 31, 0.0f, 0.0f)                                                               \
  SPRITE_DECL(S_CornerA_BR, 2, 32, 0.0f, 0.0f)                                                               \
  SPRITE_DECL(S_CornerB_TL, 2, 33, 0.0f, 0.0f)                                                               \
  SPRITE_DECL(S_CornerB_TR, 2, 34, 0.0f, 0.0f)                                                               \
  SPRITE_DECL(S_CornerB_BL, 2, 35, 0.0f, 0.0f)                                                               \
  SPRITE_DECL(S_CornerB_BR, 2, 36, 0.0f, 0.0f)                                                               \
  SPRITE_DECL(S_Hero, 9, 1, 0.0f, -4.0f)                                                                     \
  SPRITE_DECL(S_DoorClosed, 8, 1, 0.0f, 0.0f)                                                                \
  SPRITE_DECL(S_PressurePadActivated, 5, 0, 0.0f, 0.0f)                                                      \
  SPRITE_DECL(S_Warning, 5, 0, 0.0f, 0.0f)                                                                   \
  SPRITE_DECL(S_Debug4Corners, 5, 0, 0.0f, 0.0f)                                                             \
  SPRITE_DECL(S_Block, 1, 30, 0.0f, 0.0f)                                                                    \
  SPRITE_DECL(S_BlockableHole, 1, 31, 0.0f, 0.0f)                                                            \
  SPRITE_DECL(S_BlockableHoleFilled, 1, 32, 0.0f, 0.0f)                                                      \
  SPRITE_DECL(S_NumSprites, 0, 0, 0.0f, 0.0f)

#define GENERATE_ENUM(ENUM, TILE_X, TILE_Y, SO_X, SO_Y) ENUM,
typedef enum { FOREACH_SPRITE(GENERATE_ENUM) } Sprite;

// #define GENERATE_STRING(ENUM, TILE_X, TILE_Y) #ENUM,
// static const char *TILE_STRING[] = {
//     FOREACH_SPRITE(GENERATE_STRING)
// };

typedef struct {
  Dim2 location;     // location on the tileset texture (in tile space coords)
  Vec2 stage_offset; // offset (in stage pixels) when rendering
} SpriteInfo;

typedef struct {
  Dim2  location; // location of frame 0
  usize num_frames;

  f32  playback_speed;
  bool loop;

} SpriteAnimation;

typedef struct {
  Vec2 stage_offset;
  Dim2 profile_location; // location of the animated character's profile picture on the tileset texture

  SpriteAnimation idle;
  SpriteAnimation walk;
  SpriteAnimation attack;
  SpriteAnimation hit;
  SpriteAnimation death;

  bool            has_special_animation;
  SpriteAnimation special;

} AnimatedCharacterSpriteInfo;

// ANIMATED_CHARACTER_SPRITE_DECL parameters: ENUM, ROW, HAS_SPECIAL
//
#define FOREACH_ANIMATED_CHARACTER_SPRITE(ANIMATED_CHARACTER_SPRITE_DECL)                                    \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_SpearMan, 3, false)                                                     \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_CrossbowMan, 4, false)                                                  \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_BrownEagle, 5, false)                                                   \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_BlueSwordsMan, 6, false)                                                \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_BluemagicMonk, 7, false)                                                \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_BluemountedKnight, 8, false)                                            \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_BlueKnight, 9, false)                                                   \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_BlueHalberdMan, 10, false)                                              \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_BluecrossbowMan, 11, false)                                             \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_GreyEagle, 12, false)                                                   \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_BeigeSwordMan, 13, false)                                               \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_BeigemagicMonk, 14, false)                                              \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_BeigemountedKnight, 15, false)                                          \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_BeigeKnight, 16, false)                                                 \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_ElementalIce, 17, true)                                                 \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_ElementalStone, 18, true)                                               \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_ElementalRock, 19, true)                                                \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_ElementalLava, 20, true)                                                \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_ElementalMagma, 21, true)                                               \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_ElementalWTF1, 22, true)                                                \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_ElementalWTF2, 23, true)                                                \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_ElementalWTF3, 24, true)                                                \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_ElementalWTF4, 25, true)                                                \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_BoyskeletonAxe, 26, false)                                              \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_BoyzombieMachete, 27, false)                                            \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_BrownSpider, 28, false)                                                 \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_Ghoul, 29, false)                                                       \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_Vampire, 30, true)                                                      \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_VampirePriest, 31, true)                                                \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_MountedVampire, 32, false)                                              \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_BoyskeletonSword, 33, false)                                            \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_BoyzombieAxe, 34, false)                                                \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_PurpleSpider, 35, false)                                                \
  ANIMATED_CHARACTER_SPRITE_DECL(ACS_NumAnimatedCharacterSprites, 0, false)

#define GENERATE_ACS_ENUM(ENUM, ROW, HAS_SPECIAL) ENUM,
typedef enum { FOREACH_ANIMATED_CHARACTER_SPRITE(GENERATE_ACS_ENUM) } AnimatedCharacterSprite;

// #define GENERATE_STRING(ENUM, TILE_X, TILE_Y) #ENUM,
// static const char *TILE_STRING[] = {
//     FOREACH_SPRITE(GENERATE_STRING)
// };

/*
** Memory management
**
*/

typedef struct {
  void* base;
  u64   size;
  u64   used;
} BumpAllocator;

typedef struct MemoryBlock {
  usize               bytes_allocated;
  usize               bytes_requested;
  struct MemoryBlock* next;
} MemoryBlock;

// lifetimes: BumpAllocator > FixedBlockAllocator > MemoryBlock
//
// FixedBlockAllocator builds upon BumpAllocator by have 4 single linked
// lists of freed memory (binned by block size) which are used by the
// rona_malloc, rona_realloc, rona_free functions
//
typedef struct {
  BumpAllocator* bump;

  MemoryBlock* available_one_kilobyte;
  MemoryBlock* available_150_kilobyte;
  MemoryBlock* available_one_megabyte;
  MemoryBlock* available_large;
} FixedBlockAllocator;

typedef enum { ShaderType_Text, ShaderType_Tile, ShaderType_Screen } ShaderType;

typedef struct {
  GLuint program;
  int    uniform_texture;
  int    uniform_colour_fg;
  int    uniform_colour_bg;
  int    uniform_proj_matrix;
  int    uniform_pos;
} ShaderText;

typedef struct {
  GLuint program;
  int    uniform_texture;
  int    uniform_colour_fg;
  int    uniform_colour_bg;
  int    uniform_proj_matrix;
  int    uniform_pos;
} ShaderTile;

typedef struct {
  GLuint program;
  int    uniform_texture;
  int    uniform_proj_matrix;
} ShaderScreen;

#ifdef RONA_EDITOR
typedef struct {
  GLuint program;
  GLint  uniform_texture;
  GLint  uniform_proj_matrix;
} ShaderEditor;
#endif

typedef struct {
  ShaderType shader_type;
  GLuint     vao;
  GLuint     vbo; // GL_ARRAY_BUFFER for vertices
  u32        sizeof_vbo;

  GLuint ebo;          // GL_ELEMENT_ARRAY_BUFFER for indices
  i32    num_elements; // used by gl->drawElements

  f32* mesh;
  u32  mesh_size_bytes;
} Graphic;

typedef enum { EntityRole_Hero, EntityRole_Block, EntityRole_Pit } EntityRole;

typedef enum { EntityState_Standing, EntityState_Moving } EntityState;

typedef enum { EntityFacing_Left, EntityFacing_Right } EntityFacing;

typedef enum {
  EntityAnimation_Idle,
  EntityAnimation_Walk,
  EntityAnimation_Attack,
  EntityAnimation_Hit,
  EntityAnimation_Death,
  EntityAnimation_Special
} EntityAnimation;

#define ANIMATION_FRAME_COUNTER_LIMIT 10

// 0 <= Entity::z_order < ENTITY_NUM_Z_LEVELS
//
#define ENTITY_NUM_Z_LEVELS 4

typedef struct Entity {
  // when looping through entities in a level stop at the first one that doesn't exist
  // there will be no more entities where exists == true
  bool exists;

  EntityRole  entity_role;
  EntityState entity_state;

  i32 z_order;                  // 0, 1, 2, 3

  EntityFacing entity_facing;
  // -----------------------------------------------------
  // Entity Animation State
  AnimatedCharacterSprite animated_character_sprite;
  bool                    is_animated;
  EntityAnimation         entity_animation;
  f32                     animation_speed;
  i32                     animation_frame;
  i32                     animation_frame_counter; // kind of a hack, replace with time based increment
  // -----------------------------------------------------

  Vec2i board_pos;

  Vec3 world_pos;
  Vec3 world_target;
  f32  world_max_speed;
} Entity;

typedef enum { Direction_North, Direction_South, Direction_East, Direction_West } Direction;

typedef enum {
  TileType_Void,
  TileType_Floor,
} TileType;

typedef struct {
  TileType type;
  Sprite   sprite;
} Tile;

// the dimension of each chunk in tiles
#define CHUNK_WIDTH 10
#define CHUNK_HEIGHT 10

typedef struct {
  Vec2i pos; // chunk space
  Tile* tiles;
} Chunk;

typedef struct {
  Vec2i chunk_pos;
  Vec2i tile_offset;
} ChunkPos;

// --------------------------------------------------------------------------------

// Command structures for Undo Redo
//
// --------------------------------------------------------------------------------

typedef enum {
  CommandType_Delimiter = 0,
  CommandType_EntityMove,
#ifdef RONA_EDITOR
  CommandType_Editor_TileChange,
  CommandType_Editor_WallsBuild,
#endif
  CommandType_End
} CommandType;

typedef struct {
  Vec2i       board_pos;
  Vec3        world_pos;
  Vec3        world_target;
  EntityState entity_state;
} CommandParamsEntityMove;

struct Level;

typedef struct {
  ChunkPos chunk_pos;
  Tile     tile_old;
  Tile     tile_new;
} CommandParamsTileChange;

typedef struct {
  CommandType type;
  bool        is_last_in_transaction;

  Entity* entity;

  union {
    struct {
      CommandParamsEntityMove old_params;
      CommandParamsEntityMove new_params;
    } entity_move;
    struct {
      i32 some_i;
    } entity_rotate;
#ifdef RONA_EDITOR
    struct {
      struct Level* level;
      ChunkPos      chunk_pos;
      Tile          tile_old;
      Tile          tile_new;
    } editor_tile_change;
    struct {
      struct Level*            level;
      CommandParamsTileChange* changes;
    } editor_walls_build;
#endif
  } data;
} Command;

typedef struct {
  Command* sb_commands;
  bool     in_transaction;
  usize    command_index_next_free;
  usize    command_index_furthest_future;
} UndoRedo;

// --------------------------------------------------------------------------------
//
// Level
//
// --------------------------------------------------------------------------------

typedef struct Level {
  // bump_allocator manages all the memory that the level will use.
  BumpAllocator bump_allocator;
  // fixed_block_allocator references Level::bump_allocator
  FixedBlockAllocator fixed_block_allocator;

  i32     max_num_entities;
  Entity* entities;
  Graphic entities_graphic;

  // chunky tile representation
  Chunk*  chunks_sb;
  Graphic chunks_graphic;

  Rect viewport;

  // in-game undo/redo system for player moves
  //
  UndoRedo undo_redo;
} Level;

typedef struct {
  Dim2 image_dim;
  Dim2 sprite_dim;

  Vec2 uv_unit; // size of each sprite in u,v normalized co-ordinates
} Tileset;

#define STAGE_WIDTH 512
#define STAGE_HEIGHT 288
//#define STAGE_WIDTH 640
//#define STAGE_HEIGHT 360
#define TILE_DIM 16
#define TILE_WIDTH 16.0
#define TILE_HEIGHT 16.0
#define TILE_CHAR_WIDTH 8
#define TILE_CHAR_HEIGHT 16
#define HALF_TILE_WIDTH 8.0
#define HALF_TILE_HEIGHT 8.0

// X Y U V
#define TILED_VERTEX_NUM_FLOATS 4
// X Y U V R G B A R G B A
#define CHAR_VERTEX_NUM_FLOATS 12

// number of floats for each quad of geometry
#define TILED_QUAD_NUM_FLOATS (TILED_VERTEX_NUM_FLOATS * 4)
// number of floats for each character
#define CHAR_NUM_FLOATS (CHAR_VERTEX_NUM_FLOATS * 4)

// number of u32 for each quad (common for both quads and characters)
#define TILED_QUAD_NUM_INDICES 6

// (x, y, u, v) == 4 floats
// 4 * 4 bytes per float * 4 vertices per tiled quad == 64
#define TILED_QUAD_GEOMETRY_BYTES (TILED_QUAD_NUM_FLOATS * sizeof(f32))

// (x, y, u, v, fg-rgba, bg-rgba) == 12 floats
// 12 * 4 bytes per float * 4 vertices per tiled quad == 192
#define CHAR_GEOMETRY_BYTES (CHAR_NUM_FLOATS * sizeof(f32))

// 3 verts per triangle * 2 triangles * 4 bytes per i32 == 24
#define TILED_QUAD_INDICES_BYTES (TILED_QUAD_NUM_INDICES * sizeof(u32))

typedef struct {
  i32 window_width;
  i32 window_height;

  i32 stage_width;
  i32 stage_height;

  Tileset                      tileset;
  Dim2*                        char_locations;
  SpriteInfo*                  sprite_info;
  AnimatedCharacterSpriteInfo* animated_character_sprite_info;

  ShaderText   shader_text;
  ShaderTile   shader_tile;
  ShaderScreen shader_screen;
#ifdef RONA_EDITOR
  ShaderEditor shader_editor;
#endif

  GLuint tileset_texture_id;
  // GLuint font_texture_id;

  GLuint stage_texture_id;
  GLuint depth_texture_id;
  GLuint framebuffer_id;

  // text rendering
  //
  usize  max_characters_per_frame;
  usize  num_characters;
  f32*   text_vertices;
  u32*   text_indices;
  usize  text_vertices_mem_allocated;
  usize  text_indices_mem_allocated;
  GLuint text_vao;
  GLuint text_vbo;
  GLuint text_ebo;

} RenderStruct;

typedef enum { GameMode_Edit, GameMode_Play } GameMode;

typedef struct {
  BumpAllocator* bump;
  RenderStruct*  render_struct;
  Vec2           pos;
  Vec4           fg;
  Vec4           bg;
} TextParams;

typedef enum RonaButtonState {
  ButtonState_Up     = 0,
  ButtonState_Down   = 1,
  ButtonState_Repeat = 2, // this doesn't work when polling X11 from final_platform_layer
} RonaButtonState;

typedef enum RonaKey {
  Key_Space,
  Key_Return,
  Key_Shift,
  Key_Control,
  Key_Escape,

  Key_Up,
  Key_Down,
  Key_Left,
  Key_Right,

  Key_0,
  Key_1,
  Key_2,
  Key_3,
  Key_4,
  Key_5,
  Key_6,
  Key_7,
  Key_8,
  Key_9,

  Key_A,
  Key_B,
  Key_C,
  Key_D,
  Key_E,
  Key_F,
  Key_G,
  Key_H,
  Key_I,
  Key_J,
  Key_K,
  Key_L,
  Key_M,
  Key_N,
  Key_O,
  Key_P,
  Key_Q,
  Key_R,
  Key_S,
  Key_T,
  Key_U,
  Key_V,
  Key_W,
  Key_X,
  Key_Y,
  Key_Z,

  NumRonaKeys,
} RonaKey;

typedef enum RonaMouseButton {
  MouseButton_Left,
  MouseButton_Middle,
  MouseButton_Right,
  NumRonaMouseButtons
} RonaMouseButton;

typedef struct {
  bool            active;
  int             key_toggle_idx;
  RonaButtonState key[2][NumRonaKeys];
  int             mouse_toggle_idx;
  RonaButtonState mouse[2][NumRonaMouseButtons];
  Vec2i           mouse_pos;
  f32             mouse_wheel_delta;
} RonaInput;

typedef struct {
  GameMode mode;
  bool     game_initialised;
  bool     quit_game;

  TextParams text_params_debug;

  bool window_has_focus;
  bool window_resized;

  Vec2 stage_from_window_factor; // multiply this
  Vec2 stage_from_window_delta;  // add this

  RenderStruct render_struct;

  u64 time_game_start; // time at the start of the game
  u64 time_last_frame;
  u64 time_delta;

  BumpAllocator arena_permanent;
  BumpAllocator arena_transient;

  FixedBlockAllocator allocator_permanent;
  FixedBlockAllocator allocator_transient;

  Graphic screen_graphic;

  Level* level;

  RonaGL*    gl;
  RonaInput* input;
} GameState;

#endif /* RONA_H */
