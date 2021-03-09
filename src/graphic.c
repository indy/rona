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

void sprite_uv_and_offset(Vec2* uv, Vec2* offset, RenderStruct* render_struct, Sprite sprite);
void animated_character_sprite_uv_and_offset(Vec2* uv, Vec2* offset, RenderStruct* render_struct,
                                             Entity* entity);

void graphic_allocate_mesh(Graphic* graphic, BumpAllocator* bump_allocator, u32 bytes_to_allocate) {
  graphic->mesh                      = (f32*)BUMP_ALLOC(bump_allocator, bytes_to_allocate);
  graphic->mesh_allocated_size_bytes = bytes_to_allocate;
  graphic->sizeof_vbo                = 0;
  graphic->num_elements              = 0;
}

void graphic_setup_for_quads(Graphic* graphic, RonaGL* gl, BumpAllocator* transient,
                             u32 max_quads_to_render) {
  gl->genVertexArrays(1, &graphic->vao); // Vertex Array Object
  gl->bindVertexArray(graphic->vao);

  graphic->num_elements = 0;
  i32 stride            = TILED_QUAD_NUM_FLOATS;

  graphic->sizeof_vbo = sizeof(f32) * stride * max_quads_to_render;
  u32  sizeof_indices = sizeof(u32) * 6 * max_quads_to_render;
  u32* indices        = (u32*)BUMP_ALLOC(transient, sizeof_indices);

  // build indices for geometry

  // clang-format off
  for (i32 tile_count = 0; tile_count < max_quads_to_render; tile_count++) {
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

  // positions X Y
  gl->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * TILED_VERTEX_NUM_FLOATS,
                          (void*)(0 * sizeof(float)));
  // texture U V
  gl->vertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * TILED_VERTEX_NUM_FLOATS,
                          (void*)(2 * sizeof(float)));

  // gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, graphic->ebo);

  gl->bindVertexArray(0);
}

void graphic_setup_screen(Graphic* graphic, RonaGL* gl, f32 stage_width, f32 stage_height) {
  graphic->num_elements = 6;
  graphic->shader_type  = ShaderType_Screen;

  gl->genVertexArrays(1, &graphic->vao); // Vertex Array Object
  gl->bindVertexArray(graphic->vao);

  // clang-format off
  // x, y, u, v
  f32 vertices[] = {
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, stage_height, 0.0f, 1.0f,
    stage_width, stage_height, 1.0f, 1.0f,
    stage_width, 0.0f, 1.0f, 0.0f
  };
  u32 indices[] = {
    0, 1, 2,
    0, 2, 3
  };
  // clang-format on

  // the type of a Vertex Buffer Object is GL_ARRAY_BUFFER
  //
  GLuint vbo;
  gl->genBuffers(1, &vbo);
  gl->bindBuffer(GL_ARRAY_BUFFER, vbo);
  gl->bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                 GL_STATIC_DRAW); // the data is set only once and used many times.
  gl->bindBuffer(GL_ARRAY_BUFFER, 0);

  GLuint ebo;
  gl->genBuffers(1, &ebo);
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  gl->bufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  gl->bindBuffer(GL_ARRAY_BUFFER, vbo);
  gl->enableVertexAttribArray(0);
  gl->enableVertexAttribArray(1);

  // positions
  gl->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, NULL);

  // texture coords
  gl->vertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)((sizeof(float) * 2)));

  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  gl->bindVertexArray(0);
}

void graphic_teardown(Graphic* graphic, RonaGL* gl) {
  gl->deleteVertexArrays(1, &graphic->vao);
}

void graphic_tile_regenerate_geometry(Level* level, RonaGL* gl, RenderStruct* render_struct) {
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
      TilePos tile_pos = tile_pos_from_world_tile_space(vec2i(cx, cy));
      Chunk*  chunk    = chunk_ensure_get(level, tile_pos.chunk_pos);
      RONA_ASSERT(chunk);

      // the top left corner of the chunk we're about to iterate through (in world tile space)
      f32 chunk_origin_x = (f32)tile_pos.chunk_pos.x * (f32)CHUNK_WIDTH;
      f32 chunk_origin_y = (f32)tile_pos.chunk_pos.y * (f32)CHUNK_HEIGHT;

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
  // rona_log("mesh size %d, mesh_size_allocated %d", graphic->mesh_size_bytes,
  // graphic->mesh_allocated_size_bytes); rona_log("buffer_size %d, num_tiles %d, num_elements %d",
  // buffer_size, num_tiles, graphic->num_elements);

  // upload the geometry to the GPU
  //
  RONA_ASSERT(graphic->sizeof_vbo >= graphic->mesh_size_bytes);
  // rona_log("writing %d bytes to GPU", graphic->mesh_size_bytes);
  gl->bindVertexArray(graphic->vao);
  gl->bindBuffer(GL_ARRAY_BUFFER, graphic->vbo);
  gl->bufferSubData(GL_ARRAY_BUFFER, 0, graphic->mesh_size_bytes, graphic->mesh);
}

void graphic_entities_regenerate_geometry(Level* level, RonaGL* gl, RenderStruct* render_struct) {
  Tileset* tileset = &(render_struct->tileset);

  Graphic* graphic      = &(level->entities_graphic);
  graphic->num_elements = 0;
  graphic->shader_type  = ShaderType_Tile;

  f32* buffer      = graphic->mesh;
  u32  buffer_size = 0;

  u32 num_tiles = 0;

  Vec2 centre_sprite = vec2(-(f32)TILE_WIDTH / 2.0, -(f32)TILE_HEIGHT / 2.0);

  for (i32 z = 0; z < ENTITY_NUM_Z_LEVELS; z++) {
    for (i32 i = 0; i < level->max_num_entities; i++) {
      Entity* e = &(level->entities[i]);

      if (e->no_further_entities) {
        // when looping through entities in a level stop at the first no_further_entities == true
        //
        break;
      }

      if (e->ignore) {
        continue;
      }

      if (e->z_order != z) {
        // render from lowest z_order to largest
        continue;
      }

      Vec2 sprite_uv;
      Vec2 offset;

      // this is just a temporary hack, a role may have many visual representations (e.g. EntityRole_Enemy)
      switch (e->entity_role) {
      case EntityRole_Hero:
        animated_character_sprite_uv_and_offset(&sprite_uv, &offset, render_struct, e);
        break;
      case EntityRole_Block:
        sprite_uv_and_offset(&sprite_uv, &offset, render_struct, S_Block);
        break;
      case EntityRole_Pit:
        sprite_uv_and_offset(&sprite_uv, &offset, render_struct, S_BlockableHole);
        break;
      case EntityRole_FilledPit:
        sprite_uv_and_offset(&sprite_uv, &offset, render_struct, S_BlockableHoleFilled);
        break;
      case EntityRole_LevelExit:
        sprite_uv_and_offset(&sprite_uv, &offset, render_struct, S_LevelExit);
        break;
      default:
        rona_error("unknown entity_type in entities_regenerate_geometry");
        return;
        break;
      }

      f32 u  = sprite_uv.u;
      f32 v  = sprite_uv.v;
      f32 ud = tileset->uv_unit.u;
      f32 vd = tileset->uv_unit.v;

      f32 tile_origin_x = e->world_pos.x + centre_sprite.x + offset.x;
      f32 tile_origin_y = e->world_pos.y + centre_sprite.y + offset.y;

      // clang-format off
      *buffer++ = e->entity_facing == EntityFacing_Left ? tile_origin_x + TILE_WIDTH : tile_origin_x;
      *buffer++ = tile_origin_y;
      *buffer++ = u;
      *buffer++ = v;

      *buffer++ = e->entity_facing == EntityFacing_Left ? tile_origin_x + TILE_WIDTH : tile_origin_x;
      *buffer++ = tile_origin_y + TILE_HEIGHT;;
      *buffer++ = u;
      *buffer++ = v + vd;

      *buffer++ = e->entity_facing == EntityFacing_Left ? tile_origin_x : tile_origin_x + TILE_WIDTH;
      *buffer++ = tile_origin_y + TILE_HEIGHT;
      *buffer++ = u + ud;
      *buffer++ = v + vd;

      *buffer++ = e->entity_facing == EntityFacing_Left ? tile_origin_x : tile_origin_x + TILE_WIDTH;
      *buffer++ = tile_origin_y;
      *buffer++ = u + ud;
      *buffer++ = v;
      // clang-format on

      num_tiles++;
      buffer_size += TILED_QUAD_NUM_FLOATS;
      graphic->num_elements += TILED_QUAD_NUM_INDICES;
    }
  }

  graphic->mesh_size_bytes = buffer_size * sizeof(f32);

  // upload the geometry to the GPU
  //
  RONA_ASSERT(graphic->sizeof_vbo >= graphic->mesh_size_bytes);
  gl->bindVertexArray(graphic->vao);
  gl->bindBuffer(GL_ARRAY_BUFFER, graphic->vbo);
  gl->bufferSubData(GL_ARRAY_BUFFER, 0, graphic->mesh_size_bytes, graphic->mesh);
}

void sprite_uv_and_offset(Vec2* uv, Vec2* offset, RenderStruct* render_struct, Sprite sprite) {
  Tileset*    tileset     = &(render_struct->tileset);
  SpriteInfo* sprite_info = render_struct->sprite_info;

  Vec2 sprite_uv     = tileset_get_uv(tileset, sprite_info, sprite);
  Vec2 sprite_offset = sprite_info[sprite].stage_offset;

  uv->x = sprite_uv.x;
  uv->y = sprite_uv.y;

  offset->x = sprite_offset.x;
  offset->y = sprite_offset.y;
}

void animated_character_sprite_uv_and_offset(Vec2* uv, Vec2* offset, RenderStruct* render_struct,
                                             Entity* entity) {
  Tileset*                     tileset = &(render_struct->tileset);
  AnimatedCharacterSprite      acs     = entity->animated_character_sprite;
  AnimatedCharacterSpriteInfo* acsi    = &(render_struct->animated_character_sprite_info[acs]);

  offset->x = acsi->stage_offset.x;
  offset->y = acsi->stage_offset.y;

  SpriteAnimation* sprite_animation;
  switch (entity->entity_animation) {
  case EntityAnimation_Idle:
    sprite_animation = &(acsi->idle);
    break;
  case EntityAnimation_Walk:
    sprite_animation = &(acsi->walk);
    break;
  case EntityAnimation_Attack:
    sprite_animation = &(acsi->attack);
    break;
  case EntityAnimation_Hit:
    sprite_animation = &(acsi->hit);
    break;
  case EntityAnimation_Death:
    sprite_animation = &(acsi->death);
    break;
  case EntityAnimation_Special:
    sprite_animation = &(acsi->special);
    break;
  };

  entity->animation_frame_counter++;
  if (entity->animation_frame_counter == ANIMATION_FRAME_COUNTER_LIMIT) {
    entity->animation_frame_counter = 0;
    entity->animation_frame++;
    entity->animation_frame = entity->animation_frame % 4;
  }

  Dim2 sprite_location_fr0 = sprite_animation->location; // frame 0
  Vec2 sprite_uv;
  sprite_uv.x = (sprite_location_fr0.col + entity->animation_frame) * tileset->uv_unit.u;
  sprite_uv.y = sprite_location_fr0.row * tileset->uv_unit.v;

  uv->x = sprite_uv.x;
  uv->y = sprite_uv.y;
}
