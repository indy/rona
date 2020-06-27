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

// float.h for FLT_MAX
#include <float.h>
#include <math.h>

#include "cr.h"
#include "stdio.h"
#include "string.h"

#include "stdlib.h"             // temp

#include "../platform.h"
#include "../rona.h"

#include "../colour.h"
#include "../entity.h"
#include "../game.h"
#include "../input.h"
#include "../level.h"
#include "../level1.h"
#include "../memory.h"
#include "../mesh_block.h"
#include "../mesh_hero.h"
#include "../mesh_pit.h"
#include "../mesh_screen.h"
#include "../renderer.h"
#include "../rona_math.h"

static GameState *g_game_state = 0;

#define SYS_MALLOC
//#define DEBUG_MALLOC

void *stb_rona_transient_malloc(usize bytes) {
  void *addr;
#ifdef SYS_MALLOC
  addr = malloc(bytes);
#else
  addr = rona_malloc(&(g_game_state->allocator_transient), bytes);
#endif

#ifdef DEBUG_MALLOC
  RONA_LOG("called malloc %d bytes -> %p\n", bytes, addr);
#endif

  return addr;
}

void *stb_rona_transient_realloc(void* mem, usize bytes) {
  void *addr;
#ifdef SYS_MALLOC
  addr = realloc(mem, bytes);
#else
  addr = rona_realloc(&(g_game_state->allocator_transient), mem, bytes);
#endif

#ifdef DEBUG_MALLOC
  RONA_LOG("called realloc %p to %d bytes -> %p\n", mem, bytes, addr);
#endif

  return addr;
}

void stb_rona_transient_free(void* mem) {
#ifdef DEBUG_MALLOC
  RONA_LOG("called free %p\n", mem);
#endif
#ifdef SYS_MALLOC
  free(mem);
#else
  rona_free(&(g_game_state->allocator_transient), mem);
#endif
}

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ASSERT(x) RONA_ASSERT(x)
#define STBI_MALLOC(X) stb_rona_transient_malloc(X)
#define STBI_REALLOC(X, Y) stb_rona_transient_realloc(X, Y)
#define STBI_FREE(X) stb_rona_transient_free(X)
#include "stb_image.h"

#include "../colour.c"
#include "../game.c"
#include "../input.c"
#include "../level.c"
#include "../level1.c"
#include "../memory.c"
#include "../mesh_block.c"
#include "../mesh_hero.c"
#include "../mesh_pit.c"
#include "../mesh_screen.c"
#include "../renderer.c"
#include "../rona_math.c"

// To save states automatically from previous instance to a new loaded one, use CR_STATE flag on
// statics/globals. This will create a new data section in the binary for transferable states
// between instances that will be copied and kept around to overwrite the new instance initial
// states. That means that things that were initialized in a previous loaded states will continue
// from where they were.
static unsigned int CR_STATE version = 1;

CR_EXPORT int cr_main(struct cr_plugin *ctx, enum cr_op operation) {
  RONA_ASSERT(ctx);
  g_game_state = (GameState *)ctx->userdata;

  // crash protection may cause the version to decrement. So we can test current version against one
  // tracked between instances with CR_STATE to signal that we're not running the most recent
  // instance.
  if (ctx->version < version) {
    // a failure code is acessible in the `failure` variable from the `cr_plugin` context.
    // on windows this is the structured exception error code, for more info:
    //      https://msdn.microsoft.com/en-us/library/windows/desktop/ms679356(v=vs.85).aspx
    RONA_INFO("A rollback happened due to failure: %x!\n", ctx->failure);
  }
  version = ctx->version;

  switch (operation) {
  case CR_STEP:
    game_step(g_game_state);
    return 0;
  case CR_LOAD:
    RONA_INFO("LOAD (version: %d)\n", ctx->version);
    if (!g_game_state->game_initialised) {
      game_startup(g_game_state);
      g_game_state->game_initialised = true;
    }
    game_lib_load(g_game_state);
    return 0;
  case CR_UNLOAD:
    RONA_INFO("UNLOAD (version: %d)\n", ctx->version);
    game_lib_unload(g_game_state);
    return 0;
  case CR_CLOSE:
    RONA_OUT("CLOSE\n");
    game_shutdown(g_game_state);
    return 0;
  default:
    return 0;
  };
}
