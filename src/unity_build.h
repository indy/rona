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

#include "platform.h"
#include "rona.h"

#include "colour.h"
#include "entity.h"
#include "game.h"
#include "input.h"
#include "level.h"
#include "level1.h"
#include "memory.h"
#include "mesh_block.h"
#include "mesh_hero.h"
#include "mesh_pit.h"
#include "mesh_screen.h"
#include "renderer.h"
#include "rona_math.h"

#include "tileset.h"

static GameState *g_game_state = 0;

// #define SYS_MALLOC
// #define DEBUG_MALLOC

#ifdef SYS_MALLOC
#include "stdlib.h"
#endif

void *stb_rona_permanent_malloc(usize bytes) {
  void *addr;
#ifdef SYS_MALLOC
  addr = malloc(bytes);
#else
  addr = rona_malloc(&(g_game_state->allocator_permanent), bytes);
#endif

#ifdef DEBUG_MALLOC
  RONA_LOG("called malloc %d bytes -> %p\n", bytes, addr);
#endif

  return addr;
}

void *stb_rona_permanent_realloc(void *mem, usize bytes) {
  void *addr;
#ifdef SYS_MALLOC
  addr = realloc(mem, bytes);
#else
  addr = rona_realloc(&(g_game_state->allocator_permanent), mem, bytes);
#endif

#ifdef DEBUG_MALLOC
  RONA_LOG("called realloc %p to %d bytes -> %p\n", mem, bytes, addr);
#endif

  return addr;
}

void stb_rona_permanent_free(void *mem) {
#ifdef DEBUG_MALLOC
  RONA_LOG("called free %p\n", mem);
#endif
#ifdef SYS_MALLOC
  free(mem);
#else
  rona_free(&(g_game_state->allocator_permanent), mem);
#endif
}

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

void *stb_rona_transient_realloc(void *mem, usize bytes) {
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

void stb_rona_transient_free(void *mem) {
#ifdef DEBUG_MALLOC
  RONA_LOG("called free %p\n", mem);
#endif
#ifdef SYS_MALLOC
  free(mem);
#else
  rona_free(&(g_game_state->allocator_transient), mem);
#endif
}

#define RONA_PERMANENT_MALLOC(X) stb_rona_permanent_malloc(X)
#define RONA_PERMANENT_REALLOC(X, Y) stb_rona_permanent_realloc(X, Y)
#define RONA_PERMANENT_FREE(X) stb_rona_permanent_free(X)

#define RONA_TRANSIENT_MALLOC(X) stb_rona_transient_malloc(X)
#define RONA_TRANSIENT_REALLOC(X, Y) stb_rona_transient_realloc(X, Y)
#define RONA_TRANSIENT_FREE(X) stb_rona_transient_free(X)

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ASSERT(x) RONA_ASSERT(x)
#define STBI_MALLOC(X) RONA_TRANSIENT_MALLOC(X)
#define STBI_REALLOC(X, Y) RONA_TRANSIENT_REALLOC(X, Y)
#define STBI_FREE(X) RONA_TRANSIENT_FREE(X)
#include "stb_image.h"

#include "colour.c"
#include "game.c"
#include "input.c"
#include "level.c"
#include "level1.c"
#include "memory.c"
#include "mesh_block.c"
#include "mesh_hero.c"
#include "mesh_pit.c"
#include "mesh_screen.c"
#include "renderer.c"
#include "rona_math.c"
#include "tileset.c"