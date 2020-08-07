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

bool chunk_pos_eq(Vec2i chunk_pos, Chunk* chunk) {
  return vec2i_eq(chunk_pos, chunk->pos);
}

void chunktile_construct(ChunkTile* chunk_tile) {
  chunk_tile->type = TileType_Void;
  chunk_tile->sprite = TS_DebugBlank;
}

void chunk_construct(Chunk* chunk, BumpAllocator* allocator, Vec2i pos) {
  chunk->pos = pos;
  chunk->tiles = BUMP_ALLOC(allocator, sizeof(ChunkTile) * CHUNK_WIDTH * CHUNK_HEIGHT);
  RONA_ASSERT(chunk->tiles);

  for (int i = 0; i < CHUNK_WIDTH * CHUNK_HEIGHT; i++) {
    chunktile_construct(&chunk->tiles[i]);
  }
}

ChunkPos chunk_pos_from_stage_coords(Vec2i stage_coords, Vec2i viewport) {
  ChunkPos res;

  Vec2i pos_in_stage_tile_space = vec2i_div(stage_coords, vec2i(TILE_DIM, TILE_DIM));
  Vec2i pos_in_world_tile_space = vec2i_add(pos_in_stage_tile_space, viewport);

  res.chunk_pos = vec2i_div(pos_in_world_tile_space, vec2i(CHUNK_DIM, CHUNK_DIM));
  res.tile_offset = vec2i_mod(pos_in_world_tile_space, CHUNK_DIM);

  return res;
}

void chunk_pos_log(char* msg, ChunkPos cp) {
  RONA_LOG("%s: chunk_pos(%d, %d) tile_offset(%d, %d)\n", msg, cp.chunk_pos.x, cp.chunk_pos.y,
           cp.tile_offset.x, cp.tile_offset.y);
}

Chunk* chunk_ensure_get(Level* level, Vec2i chunk_pos) {
  Chunk* c = chunk_get(level->chunks, chunk_pos);

  if (!c) {
    level->chunks = sb_add(&level->fb_allocator, level->chunks, 1);
    c = &sb_last(level->chunks);
    chunk_construct(c, &level->allocator, chunk_pos);
  }

  return c;
}

Chunk* chunk_get(Chunk* chunks, Vec2i chunk_pos) {
  i32 count = sb_count(chunks);
  for (int i = 0; i < count; i++) {
    if (chunk_pos_eq(chunk_pos, &chunks[i])) {
      return &chunks[i];
    }
  }
  return NULL;
}
