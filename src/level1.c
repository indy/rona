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

/*

  | Symbol | Description |
  |--------|-------------|
  |   H    | Hero        |
  |   B    | Block       |
  |   U    | Pit         |
*/
void level1_startup(Level* level, GameState* game_state) {
// clang-format off
  // declare as: layout[height][width*2]
  #define L1_WIDTH 9
  #define L1_HEIGHT 10
  char layout[L1_HEIGHT][L1_WIDTH * 2] = {
    "    3 0 0 0 0 s 4 ",
    "    1 . . U . . 1 ",
    "    1 . B 3 0 0 6 ",
    "    1 . . 1       ",
    "3 0 6 . . 5 0 s 4 ",
    "1 H . . . U . . 1 ",
    "5 0 4 . . 3 0 0 6 ",
    "    1 . . 5 0 0 4 ",
    "    1 . . U . . 1 ",
    "    5 0 0 0 0 s 6 "
  };
  // clang-format on

  level_build(game_state, level, L1_WIDTH * 2, L1_HEIGHT, layout);
  if (!command_buffer_startup(level)) {
    RONA_ERROR("level1_startup: command_buffer_startup failed\n");
  }

#if 1
  Chunk chunk;
  chunk.pos.x = 2;
  chunk.pos.y = 4;
  chunk.tiles = NULL;
  sb_push(&level->fb_allocator, level->chunks, chunk);

  chunk.pos.x = 1;
  chunk.pos.y = 7;
  chunk.tiles = NULL;
  sb_push(&level->fb_allocator, level->chunks, chunk);

  for (int i = 0; i < sb_count(level->chunks); i++) {
    RONA_LOG("x = %d\n", level->chunks[i].pos.x);
  }

  RONA_LOG("count = %d\n", sb_count(level->chunks));
#endif

  level->viewport = rect(3, 4, 10, 10);
}

void level1_shutdown(Level* level) {
  command_buffer_shutdown(level);
  level->allocator.used = 0;
}

void level1_lib_load(Level* level, RonaGL* gl, BumpAllocator* transient, Tileset* tileset) {
  mesh_floor_lib_load(level, gl, transient, tileset);

  // Colour hero_colour;
  // colour_from(&hero_colour, ColourFormat_RGB, ColourFormat_HSLuv, 290.0f, 90.0f, 30.0f, 1.0f);

  // Entity *hero = &(level->entities[0]);
  // hero->colour.r = hero_colour.element[0];
  // hero->colour.g = hero_colour.element[1];
  // hero->colour.b = hero_colour.element[2];
  // hero->colour.a = hero_colour.element[3];
}
void level1_lib_unload(Level* level, RonaGL* gl) {
  mesh_floor_lib_unload(level, gl);
}
