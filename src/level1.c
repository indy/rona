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
void level1_startup(Level *level, GameState *game_state) {
  char layout[5][14] = {
    ". . . . . .   ",
    ". . .   U .   ",
    ". . .   . .   ",
    ". H . . B . . ",
    ". . . . . . . "
  };

  level_build(game_state, level, 14, 5, layout);
}

void level1_shutdown(Level *level) {
  level->mem.used = 0;
}

void level1_lib_load(Level *level, RonaGl *gl, MemoryArena *transient) {
  mesh_floor_lib_load(level, gl, transient);


  Colour hero_colour;
  colour_from(&hero_colour, ColourFormat_RGB, ColourFormat_HSLuv, 10.0f, 90.0f, 50.0f, 1.0f);

  Entity *hero = &(level->entities[0]);
  hero->colour.r = hero_colour.element[0];
  hero->colour.g = hero_colour.element[1];
  hero->colour.b = hero_colour.element[2];
  hero->colour.a = hero_colour.element[3];
}
void level1_lib_unload(Level *level, RonaGl *gl) {
  mesh_floor_lib_unload(level, gl);
}
