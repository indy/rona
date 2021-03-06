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

bool tile_eq(Tile t1, Tile t2) {
  return t1.type == t2.type && t1.sprite == t2.sprite && t1.placement_reason == t2.placement_reason;
}

bool tile_pos_eq(Vec2i tile_pos, Chunk* chunk) {
  return vec2i_eq(tile_pos, chunk->pos);
}

void chunktile_construct(Tile* tile) {
  tile->type   = TileType_Void;
  tile->sprite = S_Blank;
}

void chunk_construct(Chunk* chunk, BumpAllocator* bump_allocator, Vec2i pos) {
  chunk->pos   = pos;
  chunk->tiles = BUMP_ALLOC(bump_allocator, sizeof(Tile) * CHUNK_WIDTH * CHUNK_HEIGHT);
  RONA_ASSERT(chunk->tiles);

  for (int i = 0; i < CHUNK_WIDTH * CHUNK_HEIGHT; i++) {
    chunktile_construct(&chunk->tiles[i]);
  }
}

Vec2i tile_world_space_from_stage_coords(Vec2i stage_coords, Vec2i viewport_pos) {
  Vec2i pos_in_stage_tile_space = vec2i_div(stage_coords, vec2i(TILE_DIM, TILE_DIM));
  Vec2i pos_in_world_tile_space = vec2i_add(pos_in_stage_tile_space, viewport_pos);

  return pos_in_world_tile_space;
}
TilePos tile_pos_from_stage_coords(Vec2i stage_coords, Vec2i viewport_pos) {
  Vec2i pos_in_world_tile_space = tile_world_space_from_stage_coords(stage_coords, viewport_pos);

  return tile_pos_from_world_tile_space(pos_in_world_tile_space);
}

TilePos tile_pos_from_world_tile_space(Vec2i pos_in_world_tile_space) {
  TilePos res;

  res.chunk_pos   = vec2i_div(pos_in_world_tile_space, vec2i(CHUNK_WIDTH, CHUNK_HEIGHT));
  res.tile_offset = vec2i_mod(pos_in_world_tile_space, vec2i(CHUNK_WIDTH, CHUNK_HEIGHT));

  // deal with a viewport in negative tile co-ordinates
  if (pos_in_world_tile_space.x < 0 && res.tile_offset.x != 0) {
    res.chunk_pos.x -= 1;
    res.tile_offset.x += CHUNK_WIDTH;
  }
  if (pos_in_world_tile_space.y < 0 && res.tile_offset.y != 0) {
    res.chunk_pos.y -= 1;
    res.tile_offset.y += CHUNK_HEIGHT;
  }

  return res;
}

Tile* tile_from_world_tile_space(Level* level, Vec2i pos_in_world_tile_space) {
  TilePos tilepos = tile_pos_from_world_tile_space(pos_in_world_tile_space);
  Tile*   tile    = tile_ensure_get(level, tilepos);

  return tile;
}

void tile_pos_log(char* msg, TilePos cp) {
  rona_log("%s: tile_pos(%d, %d) tile_offset(%d, %d)", msg, cp.chunk_pos.x, cp.chunk_pos.y, cp.tile_offset.x,
           cp.tile_offset.y);
}

Chunk* chunk_ensure_get(Level* level, Vec2i tile_pos) {
  Chunk* chunk = NULL;
  i32    count = sb_count(level->chunks_sb);
  for (int i = 0; i < count; i++) {
    if (tile_pos_eq(tile_pos, &level->chunks_sb[i])) {
      chunk = &level->chunks_sb[i];
      break;
    }
  }

  if (!chunk) {
    chunk = sb_add(&level->fixed_block_allocator, level->chunks_sb, 1);
    chunk_construct(chunk, &level->bump_allocator, tile_pos);
  }

  RONA_ASSERT(chunk);
  return chunk;
}

Tile* tile_ensure_get(Level* level, TilePos tilepos) {
  RONA_ASSERT(level);
  RONA_ASSERT(tilepos.tile_offset.x < CHUNK_WIDTH);
  RONA_ASSERT(tilepos.tile_offset.y < CHUNK_HEIGHT);

  Chunk* chunk = chunk_ensure_get(level, tilepos.chunk_pos);
  RONA_ASSERT(chunk);

  Tile* tile = &chunk->tiles[(tilepos.tile_offset.y * CHUNK_WIDTH) + tilepos.tile_offset.x];

  return tile;
}

i32 max_number_of_renderable_tiles(Level* level) {
  i32 num_chunks_along_x = level->viewport.dim.width / CHUNK_WIDTH;
  if (num_chunks_along_x * CHUNK_WIDTH < level->viewport.dim.width) {
    num_chunks_along_x++;
  }
  i32 num_chunks_along_y = level->viewport.dim.height / CHUNK_HEIGHT;
  if (num_chunks_along_y * CHUNK_HEIGHT < level->viewport.dim.height) {
    num_chunks_along_y++;
  }
  // the max number of chunks that could be visible on the screen
  i32 max_chunks_visible = num_chunks_along_x * num_chunks_along_y;
  // the largest amount of floor geometry that could be sent to the GPU
  i32 max_tiles_to_allocate = max_chunks_visible * CHUNK_WIDTH * CHUNK_HEIGHT;

  return max_tiles_to_allocate;
}
