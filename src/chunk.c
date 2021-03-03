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

bool chunk_pos_eq(Vec2i chunk_pos, Chunk* chunk) {
  return vec2i_eq(chunk_pos, chunk->pos);
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

ChunkPos chunk_pos_from_stage_coords(Vec2i stage_coords, Vec2i viewport_pos) {
  Vec2i pos_in_stage_tile_space = vec2i_div(stage_coords, vec2i(TILE_DIM, TILE_DIM));
  Vec2i pos_in_world_tile_space = vec2i_add(pos_in_stage_tile_space, viewport_pos);

  return chunk_pos_from_world_tile_space(pos_in_world_tile_space);
}

ChunkPos chunk_pos_from_world_tile_space(Vec2i pos_in_world_tile_space) {
  ChunkPos res;

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

Tile* chunk_tile_from_world_tile_space(Level* level, Vec2i pos_in_world_tile_space) {
  ChunkPos chunkpos = chunk_pos_from_world_tile_space(pos_in_world_tile_space);
  Tile*    tile     = chunk_tile_ensure_get(level, chunkpos);

  return tile;
}

void chunk_pos_log(char* msg, ChunkPos cp) {
  rona_log("%s: chunk_pos(%d, %d) tile_offset(%d, %d)", msg, cp.chunk_pos.x, cp.chunk_pos.y,
           cp.tile_offset.x, cp.tile_offset.y);
}

Chunk* chunk_ensure_get(Level* level, Vec2i chunk_pos) {
  Chunk* chunk = NULL;
  i32    count = sb_count(level->chunks_sb);
  for (int i = 0; i < count; i++) {
    if (chunk_pos_eq(chunk_pos, &level->chunks_sb[i])) {
      chunk = &level->chunks_sb[i];
      break;
    }
  }

  if (!chunk) {
    chunk = sb_add(&level->fixed_block_allocator, level->chunks_sb, 1);
    chunk_construct(chunk, &level->bump_allocator, chunk_pos);
  }

  RONA_ASSERT(chunk);
  return chunk;
}

Tile* chunk_tile_ensure_get(Level* level, ChunkPos chunkpos) {
  RONA_ASSERT(level);
  RONA_ASSERT(chunkpos.tile_offset.x < CHUNK_WIDTH);
  RONA_ASSERT(chunkpos.tile_offset.y < CHUNK_HEIGHT);

  Chunk* chunk = chunk_ensure_get(level, chunkpos.chunk_pos);
  RONA_ASSERT(chunk);

  Tile* tile = &chunk->tiles[(chunkpos.tile_offset.y * CHUNK_WIDTH) + chunkpos.tile_offset.x];

  return tile;
}

void chunk_regenerate_geometry(Level* level, RonaGL* gl, RenderStruct* render_struct) {

  Tileset*    tileset     = &(render_struct->tileset);
  SpriteInfo* sprite_info = render_struct->sprite_info;

  Graphic* graphic      = &(level->chunks_graphic);
  graphic->num_elements = 0;
  graphic->shader_type  = ShaderType_Tile;

  f32* buffer      = graphic->mesh;
  u32  buffer_size = 0;

  u32 num_tiles = 0;

  Rect* viewport = &(level->viewport);

  for (i32 cy = viewport->pos.y; cy < viewport->pos.y + (i32)viewport->dim.height + CHUNK_HEIGHT;
       cy += CHUNK_HEIGHT) {
    for (i32 cx = viewport->pos.x; cx < viewport->pos.x + (i32)viewport->dim.width + CHUNK_WIDTH;
         cx += CHUNK_WIDTH) {
      ChunkPos chunk_pos = chunk_pos_from_world_tile_space(vec2i(cx, cy));
      Chunk*   chunk     = chunk_ensure_get(level, chunk_pos.chunk_pos);
      RONA_ASSERT(chunk);

      // the top left corner of the chunk we're about to iterate through (in world tile space)
      f32 chunk_origin_x = (f32)chunk_pos.chunk_pos.x * (f32)CHUNK_WIDTH;
      f32 chunk_origin_y = (f32)chunk_pos.chunk_pos.y * (f32)CHUNK_HEIGHT;

      for (i32 ty = 0; ty < CHUNK_HEIGHT; ty++) {
        for (i32 tx = 0; tx < CHUNK_WIDTH; tx++) {
          Tile* tile = &(chunk->tiles[(ty * CHUNK_WIDTH) + tx]);
          if (tile->sprite != S_Blank) {
            Sprite sprite = tile->sprite;

            Vec2 sprite_uv = tileset_get_uv(tileset, sprite_info, sprite);
            f32  u         = sprite_uv.u;
            f32  v         = sprite_uv.v;
            f32  ud        = tileset->uv_unit.u;
            f32  vd        = tileset->uv_unit.v;

            f32 tile_origin_x = (chunk_origin_x + tx) * TILE_WIDTH;
            f32 tile_origin_y = (chunk_origin_y + ty) * TILE_HEIGHT;

            // clang-format off
            *buffer++ = tile_origin_x;
            *buffer++ = tile_origin_y;
            *buffer++ = u;
            *buffer++ = v;

            *buffer++ = tile_origin_x;
            *buffer++ = tile_origin_y + TILE_HEIGHT;;
            *buffer++ = u;
            *buffer++ = v + vd;

            *buffer++ = tile_origin_x + TILE_WIDTH;
            *buffer++ = tile_origin_y + TILE_HEIGHT;
            *buffer++ = u + ud;
            *buffer++ = v + vd;

            *buffer++ = tile_origin_x + TILE_WIDTH;
            *buffer++ = tile_origin_y;
            *buffer++ = u + ud;
            *buffer++ = v;
            // clang-format on

            num_tiles++;
            buffer_size += TILED_QUAD_NUM_FLOATS;
            graphic->num_elements += TILED_QUAD_NUM_INDICES;
          }
        }
      }
    }
  }

  graphic->mesh_size_bytes = buffer_size * sizeof(f32);
  rona_log("num_tiles %d, num_elements %d", num_tiles, graphic->num_elements);

  // upload the geometry to the GPU
  //
  RONA_ASSERT(graphic->sizeof_vbo >= graphic->mesh_size_bytes);
  rona_log("writing %d bytes to GPU", graphic->mesh_size_bytes);
  gl->bindVertexArray(graphic->vao);
  gl->bindBuffer(GL_ARRAY_BUFFER, graphic->vbo);
  gl->bufferSubData(GL_ARRAY_BUFFER, 0, graphic->mesh_size_bytes, graphic->mesh);
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
