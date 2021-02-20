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

#ifndef LEVEL_H
#define LEVEL_H

void level_startup(Level* level, GameState* game_state);
void level_shutdown(Level* level);

bool try_moving_hero(Level* level, Entity* hero, Direction direction, GameState* game_state);
void level_build(GameState* game_state, Level* level, i32 dbl_width, i32 height,
                 char layout[][dbl_width]);

void world_from_board(Vec3* dst, i32 x, i32 y, f32 z);

#endif /* LEVEL_H */
