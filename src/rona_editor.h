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

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

#define UNUSED(a) (void)a
#define LEN(a) (sizeof(a) / sizeof(a)[0])

#ifdef __APPLE__
#define NK_SHADER_VERSION "#version 150\n"
#else
#define NK_SHADER_VERSION "#version 300 es\n"
#endif

typedef struct {
  // use the level's allocator for undo/redo
  UndoRedo undo_redo;

  // _temporary_ tile selection
  int active_tile_type; // 0 == void, 1 == floor

  // position of cursor on the stage (valid if in range 0..STAGE_WIDTH, 0..STAGE_HEIGHT)
  Vec2i cursor_in_stage_coords;
  // how much should the stage be magnified
  i32 stage_scalar;

  // -----------------------------------

  struct nk_font_atlas atlas;
  struct nk_context    ctx;

  usize nuklear_memory_size;
  void* nuklear_memory;

  struct nk_font* default_font;

  // -----------------------------------------------------------------------------
  //
  // memory management variables for font atlas functionality
  //
  BumpAllocator bump_permanent; // reserves MEMORY_ALLOCATION_NUKLEAR_ATLAS from arena_permanent
  FixedBlockAllocator allocator_permanent;

  BumpAllocator       bump_transient; // reserves transient memory, so should be used with caution
  FixedBlockAllocator allocator_transient;

  bool transient_allocation_calls_expected;
  // -----------------------------------------------------------------------------

  // render the stage_texture onto stage_in_nuklear_texture_id applying colour space conversion
  GLuint stage_in_nuklear_texture_id;
  GLuint depth_texture_id;
  GLuint framebuffer_id;

  struct nk_buffer            cmds;
  struct nk_draw_null_texture null;

  GLuint vbo, vao, ebo;
  GLuint font_tex;

} EditorState;

static EditorState CR_STATE editor_state;

void* nuklear_persistent_alloc(nk_handle h, void* mem, nk_size bytes);
void  nuklear_persistent_free(nk_handle h, void* mem);
void* nuklear_transient_alloc(nk_handle h, void* mem, nk_size bytes);
void  nuklear_transient_free(nk_handle h, void* mem);

void editor_startup(RonaGL* gl, EditorState* editor_state, BumpAllocator* permanent,
                    BumpAllocator* transient);
void editor_shutdown(RonaGL* gl, EditorState* dev);

void editor_lib_load(RonaGL* gl, EditorState* editor_state, ShaderEditor* shader_editor);
void editor_lib_unload(RonaGL* gl, EditorState* editor_state);

void editor_changed_level(EditorState* editor_state, Level* level);

void editor_render(RonaGL* gl, EditorState* editor_state, int width, int height,
                   ShaderEditor* shader_editor);
void editor_step(EditorState* editor_state, GameState* game_state);
