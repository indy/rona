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
  struct nk_font_atlas atlas;
  struct nk_context    ctx;

  usize nuklear_memory_size;
  void* nuklear_memory;

  struct nk_font* default_font;

  // -----------------------------------------------------------------------------
  //
  // memory management variables for font atlas functionality
  //
  struct nk_allocator persistent; // todo: can't these nk_allocators just be local variables?
  struct nk_allocator transient;

  BumpAllocator    bump_permanent; // reserves MEMORY_ALLOCATION_NUKLEAR_ATLAS from arena_permanent
  GroupedAllocator allocator_permanent;

  BumpAllocator    bump_transient; // reserves transient memory, so should be used with caution
  GroupedAllocator allocator_transient;

  bool transient_allocation_calls_expected;
  // -----------------------------------------------------------------------------

  // render the stage_texture onto stage_in_nuklear_texture_id applying colour space conversion
  GLuint stage_in_nuklear_texture_id;
  GLuint depth_texture_id;
  GLuint framebuffer_id;

  struct nk_buffer            cmds;
  struct nk_draw_null_texture null;

  GLuint                      vbo, vao, ebo;

  GLuint                      font_tex;

} EditorState;

static EditorState CR_STATE editor_state;

void* nuklear_persistent_alloc(nk_handle h, void* mem, nk_size bytes);
void  nuklear_persistent_free(nk_handle h, void* mem);
void* nuklear_transient_alloc(nk_handle h, void* mem, nk_size bytes);
void  nuklear_transient_free(nk_handle h, void* mem);
