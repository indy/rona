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

#include "rona.h"

#include "cr.h"

static GameState* g_game_state = 0;

#include "memory.h"

// #define SYS_MALLOC
// #define DEBUG_MALLOC

#ifndef RONA_EDITOR
#include <stdio.h>
#endif

#ifdef SYS_MALLOC
#include "stdlib.h"
#endif

#ifdef RONA_EDITOR

#define STBTT_malloc(x, u) rona_permanent_malloc(x)
#define STBTT_free(x, u) rona_permanent_free(x)

#define NK_ASSERT RONA_ASSERT

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION

#include "nuklear.h"
#include "rona_editor.h"

#endif /* RONA_EDITOR */

#include "chunk.h"
#include "colour.h"
#include "command.h"
#include "entity.h"
#include "game.h"
#include "graphic.h"
#include "graphic_screen.h"
#include "input.h"
#include "level.h"
#include "level1.h"
#include "renderer.h"
#include "rona_math.h"
#include "stretchy.h"
#include "tileset.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ASSERT(x) RONA_ASSERT(x)
#define STBI_MALLOC(X) rona_transient_malloc(X)
#define STBI_REALLOC(X, Y) rona_transient_realloc(X, Y)
#define STBI_FREE(X) rona_transient_free(X)
#include "stb_image.h"

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

#ifdef RONA_EDITOR
#include "rona_editor.c"
#endif

#include "chunk.c"
#include "colour.c"
#include "command.c"
#include "game.c"
#include "graphic.c"
#include "graphic_screen.c"
#include "input.c"
#include "level.c"
#include "level1.c"
#include "memory.c"
#include "renderer.c"
#include "rona_math.c"
#include "tileset.c"
