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
  tile->type = TileType_Void;
  tile->sprite = TS_DebugBlank;
}

void chunk_construct(Chunk* chunk, BumpAllocator* bump_allocator, Vec2i pos) {
  chunk->pos = pos;
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

  res.chunk_pos = vec2i_div(pos_in_world_tile_space, vec2i(CHUNK_WIDTH, CHUNK_HEIGHT));
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
  Tile*    tile = chunk_tile_ensure_get(level, chunkpos);

  return tile;
}

void chunk_pos_log(char* msg, ChunkPos cp) {
  RONA_LOG("%s: chunk_pos(%d, %d) tile_offset(%d, %d)\n", msg, cp.chunk_pos.x, cp.chunk_pos.y,
           cp.tile_offset.x, cp.tile_offset.y);
}

Chunk* chunk_ensure_get(Level* level, Vec2i chunk_pos) {
  Chunk* chunk = NULL;
  i32    count = sb_count(level->sb_chunks);
  for (int i = 0; i < count; i++) {
    if (chunk_pos_eq(chunk_pos, &level->sb_chunks[i])) {
      chunk = &level->sb_chunks[i];
      break;
    }
  }

  if (!chunk) {
    chunk = sb_add(&level->fixed_block_allocator, level->sb_chunks, 1);
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

void chunk_regenerate_geometry(Level* level, RonaGL* gl, Tileset* tileset) {
  f32* buffer = level->chunk_mesh;
  u32  buffer_size = 0;

  // temp
  Colour bg_col = colour_make(ColourFormat_RGB, 50.0f, 20.0f, 80.0f, 1.0f);
  Colour fg_col = colour_make(ColourFormat_HSLuv, 400.0f, 90.0f, 30.0f, 1.0f);
  Vec4   fg, bg;
  Colour c;
  vec4_from_colour(&fg, colour_clone_as(&c, &fg_col, ColourFormat_RGB));
  vec4_from_colour(&bg, colour_clone_as(&c, &bg_col, ColourFormat_RGB));

  Graphic* graphic = &(level->chunk_graphic);
  graphic->num_elements = 0;
  graphic->shader_type = ShaderType_Tile;

  u32 num_tiles = 0;

  Rect* viewport = &(level->viewport);
  for (i32 cy = viewport->pos.y; cy < viewport->pos.y + (i32)viewport->dim.height; cy += CHUNK_HEIGHT) {
    for (i32 cx = viewport->pos.x; cx < viewport->pos.x + (i32)viewport->dim.width; cx += CHUNK_WIDTH) {
      ChunkPos chunk_pos = chunk_pos_from_world_tile_space(vec2i(cx, cy));
      Chunk*   chunk = chunk_ensure_get(level, chunk_pos.chunk_pos);
      RONA_ASSERT(chunk);

      // the top left corner of the chunk we're about to iterate through (in world tile space)
      f32 chunk_origin_x = (f32)chunk_pos.chunk_pos.x * (f32)CHUNK_WIDTH;
      f32 chunk_origin_y = (f32)chunk_pos.chunk_pos.y * (f32)CHUNK_HEIGHT;

      for (i32 ty = 0; ty < CHUNK_HEIGHT; ty++) {
        for (i32 tx = 0; tx < CHUNK_WIDTH; tx++) {
          Tile* tile = &(chunk->tiles[(ty * CHUNK_WIDTH) + tx]);
          if (tile->sprite != TS_DebugBlank) {
            TilesetSprite tile_sprite = tile->sprite;

            Vec2 sprite = tileset_get_uv(tileset, tile_sprite);
            f32  u = sprite.u;
            f32  v = sprite.v;
            f32  ud = tileset->uv_unit.u;
            f32  vd = tileset->uv_unit.v;

            f32 tile_origin_x = (chunk_origin_x + tx) * TILE_WIDTH;
            f32 tile_origin_y = (chunk_origin_y + ty) * TILE_HEIGHT;

            // clang-format off
            *buffer++ = tile_origin_x;
            *buffer++ = tile_origin_y;
            *buffer++ = u;
            *buffer++ = v;
            *buffer++ = fg.e[0]; *buffer++ = fg.e[1]; *buffer++ = fg.e[2]; *buffer++ = fg.e[3];
            *buffer++ = bg.e[0]; *buffer++ = bg.e[1]; *buffer++ = bg.e[2]; *buffer++ = bg.e[3];

            *buffer++ = tile_origin_x;
            *buffer++ = tile_origin_y + TILE_HEIGHT;;
            *buffer++ = u;
            *buffer++ = v + vd;
            *buffer++ = fg.e[0]; *buffer++ = fg.e[1]; *buffer++ = fg.e[2]; *buffer++ = fg.e[3];
            *buffer++ = bg.e[0]; *buffer++ = bg.e[1]; *buffer++ = bg.e[2]; *buffer++ = bg.e[3];

            *buffer++ = tile_origin_x + TILE_WIDTH;
            *buffer++ = tile_origin_y + TILE_HEIGHT;
            *buffer++ = u + ud;
            *buffer++ = v + vd;
            *buffer++ = fg.e[0]; *buffer++ = fg.e[1]; *buffer++ = fg.e[2]; *buffer++ = fg.e[3];
            *buffer++ = bg.e[0]; *buffer++ = bg.e[1]; *buffer++ = bg.e[2]; *buffer++ = bg.e[3];

            *buffer++ = tile_origin_x + TILE_WIDTH;
            *buffer++ = tile_origin_y;
            *buffer++ = u + ud;
            *buffer++ = v;
            *buffer++ = fg.e[0]; *buffer++ = fg.e[1]; *buffer++ = fg.e[2]; *buffer++ = fg.e[3];
            *buffer++ = bg.e[0]; *buffer++ = bg.e[1]; *buffer++ = bg.e[2]; *buffer++ = bg.e[3];
            // clang-format on

            num_tiles++;
            buffer_size += TILED_QUAD_NUM_FLOATS;
            graphic->num_elements += TILED_QUAD_NUM_INDICES;
          }
        }
      }
    }
  }

  level->chunk_mesh_size_bytes = buffer_size * sizeof(f32);
  RONA_LOG("num_tiles %d, num_elements %d\n", num_tiles, graphic->num_elements);

  // upload the geometry to the GPU
  //
  RONA_ASSERT(graphic->sizeof_vbo >= level->chunk_mesh_size_bytes);
  RONA_LOG("writing %d bytes to GPU\n", level->chunk_mesh_size_bytes);
  gl->bindVertexArray(graphic->vao);
  gl->bindBuffer(GL_ARRAY_BUFFER, graphic->vbo);
  gl->bufferSubData(GL_ARRAY_BUFFER, 0, level->chunk_mesh_size_bytes, level->chunk_mesh);
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

void chunk_startup(Level* level) {
  // allocate the memory for the tile geometry
  u32 memory_to_allocate = max_number_of_renderable_tiles(level) * TILED_QUAD_NUM_FLOATS;
  RONA_LOG("chunk_startup: memory_to_allocate: %u\n", memory_to_allocate);
  level->chunk_mesh = (f32*)BUMP_ALLOC(&level->bump_allocator, memory_to_allocate);

  level->chunk_graphic.sizeof_vbo = 0;
  level->chunk_graphic.num_elements = 0;
}

void chunk_shutdown(Level* level) {
}

void chunk_lib_load(Level* level, RonaGL* gl, BumpAllocator* transient) {
  Graphic* graphic = &(level->chunk_graphic);

  gl->genVertexArrays(1, &graphic->vao); // Vertex Array Object
  gl->bindVertexArray(graphic->vao);

  i32 num_floor_tiles = max_number_of_renderable_tiles(level);

  graphic->num_elements = 0;
  i32 stride = TILED_QUAD_NUM_FLOATS;

  graphic->sizeof_vbo = sizeof(f32) * stride * num_floor_tiles;
  u32  sizeof_indices = sizeof(u32) * 6 * num_floor_tiles;
  u32* indices = (u32*)BUMP_ALLOC(transient, sizeof_indices);

  // build indices for geometry

  // clang-format off
  for (i32 tile_count = 0; tile_count < num_floor_tiles; tile_count++) {
    i32 i_index = tile_count * 6;
    i32 offset = tile_count * 4;
    indices[i_index + 0] = 0 + offset;
    indices[i_index + 1] = 1 + offset;
    indices[i_index + 2] = 2 + offset;
    indices[i_index + 3] = 0 + offset;
    indices[i_index + 4] = 2 + offset;
    indices[i_index + 5] = 3 + offset;
  }
  // clang-format on

  // the type of a Vertex Buffer Object is GL_ARRAY_BUFFER
  //
  gl->genBuffers(1, &graphic->vbo);
  gl->bindBuffer(GL_ARRAY_BUFFER, graphic->vbo);
  gl->bufferData(GL_ARRAY_BUFFER, graphic->sizeof_vbo, 0,
                 GL_DYNAMIC_DRAW); // the data is set only once and used many times.
  // gl->bindBuffer(GL_ARRAY_BUFFER, 0);

  gl->genBuffers(1, &graphic->ebo);
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, graphic->ebo);
  gl->bufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof_indices, indices, GL_DYNAMIC_DRAW);
  // gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  gl->bindBuffer(GL_ARRAY_BUFFER, graphic->vbo);
  gl->enableVertexAttribArray(0);
  gl->enableVertexAttribArray(1);
  gl->enableVertexAttribArray(2);
  gl->enableVertexAttribArray(3);

  // positions
  gl->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (void*)(0 * sizeof(float)));
  // uv
  gl->vertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (void*)(2 * sizeof(float)));

  gl->vertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (void*)(4 * sizeof(float)));
  gl->vertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (void*)(8 * sizeof(float)));

  // gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, graphic->ebo);

  gl->bindVertexArray(0);
}

void chunk_lib_unload(Level* level, RonaGL* gl) {
  gl->deleteVertexArrays(1, &level->chunk_graphic.vao);
}
