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

#define MEMORY_COMMANDS_IN_BUFFER 10000

#ifdef _DEBUG
#define RONA_ASSERT(exp)                                                                           \
  if (!(exp)) {                                                                                    \
    fprintf(stderr, "ASSERT FAILURE: %s (%s:%d)\n", __func__, __FILE__, __LINE__);                 \
    fflush(stderr);                                                                                \
    *(int*)0 = 0;                                                                                  \
  }
#else
#define RONA_ASSERT(exp)                                                                           \
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

typedef enum {
  TS_Block = 0,

  TS_Hero,
  TS_Archer,
  TS_Warrior,
  TS_Cleric,
  TS_Rogue,
  TS_Sorcerer,
  TS_Mage,
  TS_King,
  TS_Boy,
  TS_Dog,
  TS_Toddler,
  TS_Fatman,
  TS_Ghost,

  TS_DebugChecker,
  TS_DebugBlank,
  TS_DebugFilled,
  TS_Debug4Corners,

  TS_BorderTop,
  TS_BorderTopRight,
  TS_BorderRight,
  TS_BorderBottomRight,
  TS_BorderBottom,
  TS_BorderLeft,
  TS_BorderBottomLeft,
  TS_BorderTopLeft,

  TS_Heroine,
  TS_FemaleArcher,
  TS_FemaleWarrior,
  TS_FemaleCleric,
  TS_FemaleRogue,
  TS_Sorceress,
  TS_FemaleMage,
  TS_Queen,
  TS_Girl,
  TS_Cat,
  TS_FemaleToddler,
  TS_Fatwoman,
  TS_FemaleGhost,

  TS_Floor,

  TS_OldMan,
  TS_Patches,
  TS_Hopolite,
  TS_MagicThing1,
  TS_MagicOldMan,
  TS_MagicThing2,
  TS_Mermaid,

  TS_BlockSub0,
  TS_BlockSub1,
  TS_BlockSub2,
  TS_BlockPat1,
  TS_BlockPat2,

  TS_FaceNeutral,
  TS_FaceSmile,
  TS_FaceJoy,
  TS_FaceSad,
  TS_FaceCry,
  TS_FaceAngry,
  TS_FaceShout,
  TS_FaceTaste,
  TS_FaceEat,
  TS_FaceSleep,
  TS_FacePatches,
  TS_FaceGlasses,

  TS_Rat,
  TS_Bat,
  TS_Caterpillar,
  TS_Spider,
  TS_Scorpion,
  TS_Snail,
  TS_Snake,
  TS_Dingo,
  TS_Panther,
  TS_Bear,
  TS_Drake,
  TS_Dragon,
  TS_Unicorn,

  TS_Gremlin,
  TS_Ogre,
  TS_Troll,
  TS_Minotaur,

  TS_Ghoul,
  TS_PhantomHand,
  TS_Skeleton,
  TS_SkeletonWarrior,
  TS_SkeletonDeath,
  TS_EnemyGhost,
  TS_GhostSpirit,
  TS_Phantom,

  TS_JellyFish,
  TS_Octopus,
  TS_Eyeball,
  TS_MidgetThief,
  TS_MidgetSprite,
  TS_WalkingChest,
  TS_Robot,
  TS_RobotBrute,

  TS_Brick1,
  TS_Brick2,
  TS_Brick3,
  TS_Ground1,
  TS_Ground2,
  TS_Ground3,
  TS_PressurePadPrimed,
  TS_PressurePadActivated,
  TS_PointyGround,
  TS_FloorSwitch,
  TS_StairsDown,
  TS_StairsUp,
  TS_DoorClosed,
  TS_DoorOpen,
  TS_FlamingTurd,
  TS_SquashedTurd,

  TS_Gravestone,
  TS_Sign,
  TS_Stool,
  TS_Cupboard,
  TS_Bookshelf,
  TS_Table,
  TS_Bed,

  TS_WallHorizontal,
  TS_WallVertical,
  TS_WallHorizontalPoint,
  TS_WallTLCorner,
  TS_WallTRCorner,
  TS_WallBLCorner,
  TS_WallBRCorner,
  TS_WallTeeLeft,
  TS_WallTeeRight,
  TS_WallTeeDown,
  TS_WallTeeUp,
  TS_WallCross,

  TS_WeakFloor,
  TS_SpiderWeb,
  TS_TrapPrimed,
  TS_TrapActivated,
  TS_SpikesOnFloor,
  TS_SpikesInFloor,
  TS_Grate,
  TS_Wat,
  TS_FloorSwitchPrimed,
  TS_FloorSwitchActivated,
  TS_LeverLeft,
  TS_LeverRight,

  TS_MapPlain1,
  TS_MapPlain2,
  TS_MapPlain3,
  TS_MapTree1,
  TS_MapTree2,
  TS_MapTree3,
  TS_MapWater,
  TS_MapHouses,
  TS_MapCave,
  TS_MapHouse,
  TS_MapCastle,
  TS_MapCaravan,
  TS_MapShip,

  TS_Torch,
  TS_Lantern,
  TS_Spade,
  TS_Pickaxe,
  TS_Rope,
  TS_Bomb,
  TS_ChestClosed,
  TS_ChestOpen,
  TS_Vase,
  TS_VaseBroken,
  TS_DoorKey,
  TS_ChestKey,
  TS_Pouch,
  TS_Coins,
  TS_Biscuit,
  TS_Rupee,

  TS_Pearl,
  TS_SkullAndBones,
  TS_Bone,
  TS_Tongue,
  TS_MonsterSkull,

  TS_FoodDrumStick,
  TS_FoodSteak,
  TS_FoodKebab,
  TS_FoodFish,
  TS_FoodFriedEgg,
  TS_FoodBreadSlice,
  TS_FoodEgg,
  TS_FoodQuiche,
  TS_FoodApple,
  TS_FoodPumpkin,
  TS_FoodTurnip,
  TS_FoodBasil,
  TS_FoodFlower,
  TS_FoodMushroom,
  TS_FoodSweet,
  TS_FoodCake,

  TS_Beer,
  TS_Pot,
  TS_Jug,
  TS_SmallJar,
  TS_LargeJar,
  TS_ExpensiveJar,

  TS_WeaponDagger,
  TS_WeaponSword,
  TS_WeaponAxe,
  TS_WeaponPickaxe,
  TS_WeaponStaff,
  TS_WeaponClub,
  TS_WeaponMallet,
  TS_WeaponShield1,
  TS_WeaponShield2,
  TS_WeaponBow,
  TS_WeaponArrows,
  TS_ArmourHelmet1,
  TS_ArmourHelmet2,
  TS_WizardHat,
  TS_ArmourGloves,
  TS_ArmourBracelets,

  TS_ClothesBoots,
  TS_ClothesTrousers,
  TS_ClothesBelt,
  TS_ClothesTop1,
  TS_ClothesTop2,
  TS_ClothesTop3,
  TS_ClothesHood,
  TS_ClothesRing,
  TS_ClothesPendant,
  TS_ClothesCrown,
  TS_ClothesGlasses,

  TS_MagickWand,
  TS_MagickWandJeweled,
  TS_MagickWandSkulled,
  TS_MagickGlobe,
  TS_MagickPendant,
  TS_MagickSkull,
  TS_MagickScroll1,
  TS_MagickScroll2,
  TS_MagickBook,
  TS_MagickPedestal,
  TS_MagickCouldron,
  TS_MagickHex1,
  TS_MagickHex2,
  TS_MagickHexHand,
  TS_MagickWat,

  TS_MusicMandolin,
  TS_MusicHarp,
  TS_MusicPanFlute,
  TS_MusicBell,
  TS_MusicXylophone,
  TS_MusicDrum,

  TS_At,
  TS_DontUse1,
  TS_DontUse2,
  TS_Plus,
  TS_Cross,
  TS_Heart,
  TS_Star,
  TS_Sun,
  TS_Moon,
  TS_Bullseye,
  TS_Warning,
  TS_Music,
  TS_Fire,
  TS_Frost,
  TS_Rain,
  TS_Lightning,

  TS_Whirl,
  TS_Bubble,
  TS_Sleep,
  TS_Skull,
  TS_Arms,
  TS_Shield,
  TS_Hourglass,
  TS_North,
  TS_NorthEast,
  TS_East,
  TS_SouthEast,
  TS_South,
  TS_SouthWest,
  TS_West,
  TS_NorthWest,

  TS_NumTilesetSprites
} TilesetSprite;

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
    struct {
      i32 width;
      i32 height;
    };
    i32 e[2];
  };
} Vec2i;

typedef struct {
  i32   x;
  i32   y;
  usize width;
  usize height;
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

typedef enum { ShaderType_Tile, ShaderType_Screen } ShaderType;

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

  i32 num_elements; // used by gl->drawElements
} Mesh;

typedef enum { EntityType_Hero, EntityType_Block, EntityType_Pit } EntityType;

typedef enum { EntityState_Standing, EntityState_Moving } EntityState;

typedef struct Entity {
  EntityType  entity_type;
  EntityState entity_state;

  // when looping through entities in a level stop at the first one that doesn't exist
  // there will be no more entites where exists == true
  bool exists;

  Mesh* mesh;
  Vec4  colour;

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
  TileType      type;
  TilesetSprite sprite;
} Tile;

// the dimension of each chunk in tiles
#define CHUNK_DIM 10
#define CHUNK_WIDTH 10
#define CHUNK_HEIGHT 10

typedef struct {
  TileType      type;
  TilesetSprite sprite;
} ChunkTile;

typedef struct {
  Vec2i      pos; // chunk space
  ChunkTile* tiles;
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
  CommandType_Editor_ChangeTile,
#endif
  CommandType_End
} CommandType;

typedef struct {
  Vec2i       board_pos;
  Vec3        world_pos;
  Vec3        world_target;
  EntityState entity_state;
} CommandParamsEntityMove;

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
      ChunkPos  chunk_pos;
      ChunkTile old_tile;
      ChunkTile new_tile;
    } editor_change_tile;
#endif
  } data;
} Command;

typedef struct CommandBuffer {
  Command* command;

  struct CommandBuffer* prev;
  struct CommandBuffer* next;
  usize                 size; // size is in terms of number of Command structs in *command
  usize                 used; // used is in number of Command structs
} CommandBuffer;

typedef struct {
  bool           in_command_transaction;
  usize          command_index_next_free;       // the next command in the current command_buffer
  CommandBuffer* command_buffer;                // the active command buffer
  usize          command_index_furthest_future; // index of the furthest future point
  CommandBuffer*
      command_buffer_furthest_future; // buffer that contains command_index_furthest_future
} UndoRedo;

// --------------------------------------------------------------------------------
//
// Level
//
// --------------------------------------------------------------------------------

typedef struct {
  BumpAllocator       allocator;
  FixedBlockAllocator fb_allocator;

  i32     max_num_entities;
  Entity* entities;

  // chunky tile representation
  Chunk* chunks; // stretchy buffer
  Rect   viewport;

  // old tile representation
  i32   width;
  i32   height;
  Tile* tiles;
  Vec2  offset_stage_from_world;

  // mesh
  Mesh* mesh_floor;

  // in-game undo/redo system for player moves
  //
  UndoRedo undo_redo;
} Level;

typedef struct {
  // char *image_filename;
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

#define TILED_VERTEX_NUM_FLOATS_FOR_GEOMETRY 12

// number of floats for each quad of geometry
#define TILED_QUAD_GEOMETRY_SIZEOF_1 (TILED_VERTEX_NUM_FLOATS_FOR_GEOMETRY * 4)
// number of u32 for each quad
#define TILED_QUAD_INDICES_SIZEOF_1 6

// (x, y, u, v, fg-rgba, bg-rgba) == 12 bytes
// 12 * 4 bytes per float * 4 vertices per tiled quad == 192
#define TILED_QUAD_GEOMETRY_BYTES (TILED_QUAD_GEOMETRY_SIZEOF_1 * 4)
// 3 verts per triangle * 2 triangles * 4 bytes per i32 == 24
#define TILED_QUAD_INDICES_BYTES (TILED_QUAD_INDICES_SIZEOF_1 * 4)

typedef struct {
  i32 window_width;
  i32 window_height;

  i32 stage_width;
  i32 stage_height;

  Tileset tileset;
  // Tileset font_tileset;

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
  ButtonState_Up = 0,
  ButtonState_Down = 1,
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

  Mesh* mesh_screen;
  Mesh* mesh_hero;
  Mesh* mesh_block;
  Mesh* mesh_pit;

  Level* level;

  RonaGL*    gl;
  RonaInput* input;
} GameState;

#endif /* RONA_H */
