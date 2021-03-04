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

#ifndef LEVEL_H
#define LEVEL_H

void level_startup(Level* level, GameState* game_state);
void level_shutdown(Level* level);

void level_lib_load(Level* level, RonaGL* gl, BumpAllocator* transient, RenderStruct* render_struct);
void level_lib_unload(Level* level, RonaGL* gl);

bool try_moving_hero(Level* level, Entity* hero, Direction direction, GameState* game_state);
void level_build(GameState* game_state, Level* level, i32 dbl_width, i32 height, char layout[][dbl_width]);

#endif /* LEVEL_H */
