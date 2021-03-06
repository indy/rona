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

#ifndef TILE_H
#define TILE_H

i32 max_number_of_renderable_tiles(Level* level);

void tile_pos_log(char* msg, TilePos cp);
#define TILEPOS_LOG(x) tile_pos_log(#x, x)
TilePos tile_pos_from_stage_coords(Vec2i stage_coords, Vec2i viewport);
TilePos tile_pos_from_world_tile_space(Vec2i pos_in_world_tile_space);

Vec2i tile_world_space_from_stage_coords(Vec2i stage_coords, Vec2i viewport_pos);

Tile* tile_ensure_get(Level* level, TilePos tilepos);
Tile* tile_from_world_tile_space(Level* level, Vec2i pos_in_world_tile_space);

bool tile_eq(Tile t1, Tile t2);

Chunk* chunk_ensure_get(Level* level, Vec2i tile_pos);

#endif // TILE_H
