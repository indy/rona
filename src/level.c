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

Handle g_entity_handle;
void   entity_defaults(Entity* entity, EntityRole entity_role);

void level_startup(Level* level, GameState* game_state) {
  BumpAllocator* bump_allocator = &(level->bump_allocator);

  usize entities_graphic_memory_bytes =
      LEVEL_RESERVE_MAX_ENTITIES_TO_RENDER * TILED_QUAD_NUM_FLOATS * sizeof(f32);
  rona_log("allocating %llu bytes for entities_graphic_memory_bytes", entities_graphic_memory_bytes);
  graphic_allocate_mesh(&level->entities_graphic, bump_allocator, entities_graphic_memory_bytes);

  usize chunks_graphic_memory_bytes =
      max_number_of_renderable_tiles(level) * TILED_QUAD_NUM_FLOATS * sizeof(f32);
  rona_log("allocating %llu bytes for chunks_graphic_memory_bytes", chunks_graphic_memory_bytes);
  graphic_allocate_mesh(&level->chunks_graphic, bump_allocator, chunks_graphic_memory_bytes);
}

void level_shutdown(Level* level) {
}

void level_lib_load(Level* level, RonaGL* gl, BumpAllocator* transient, RenderStruct* render_struct) {
  graphic_setup_for_quads(&level->entities_graphic, gl, transient, LEVEL_RESERVE_MAX_ENTITIES_TO_RENDER);

  graphic_setup_for_quads(&level->chunks_graphic, gl, transient, max_number_of_renderable_tiles(level));
  graphic_tile_regenerate_geometry(level, gl, render_struct);
}

void level_lib_unload(Level* level, RonaGL* gl) {
  graphic_teardown(&level->chunks_graphic, gl);
  graphic_teardown(&level->entities_graphic, gl);
}

void world_from_board(Vec3* dst, i32 x, i32 y, f32 z) {
  vec3_set(dst, ((f32)x * TILE_WIDTH) + HALF_TILE_WIDTH, ((f32)y * TILE_HEIGHT) + HALF_TILE_HEIGHT, z);
}

// place an entity at the given board positions
void entity_place(Level* level, Entity* entity, i32 board_pos_x, i32 board_pos_y, f32 z) {
  vec2i_set(&entity->board_pos, board_pos_x, board_pos_y);
  world_from_board(&entity->world_pos, board_pos_x, board_pos_y, z);
  world_from_board(&entity->world_target, board_pos_x, board_pos_y, z);
}

Sprite floor_sprite(i32 i) {
  Sprite sprites[]   = {S_Floor01, S_Floor02, S_Floor03, S_Floor04, S_Floor05};
  i32    num_sprites = 5;

  return sprites[i % num_sprites];
}

Sprite wall_exterior_sprite(i32 i) {
  Sprite sprites[]   = {S_Warning, S_Warning};
  i32    num_sprites = 2;

  return sprites[i % num_sprites];
}

void level_build(GameState* game_state, Level* level, i32 dbl_width, i32 height, char layout[][dbl_width]) {

  level->chunks_sb = NULL;

  level->max_num_entities = 10;
  level->entities = (Entity*)BUMP_ALLOC(&(level->bump_allocator), sizeof(Entity) * level->max_num_entities);
  for (i32 i = 0; i < level->max_num_entities; i++) {
    level->entities[i].no_further_entities = true;
  }

  g_entity_handle = 0;

  bool have_hero                  = false;
  i32  next_non_hero_entity_index = 1;

  i32 sprite_count = 0;

  for (i32 j = 0; j < height; j++) {

    char* plan_line = layout[j];

    for (i32 i = 0; i < dbl_width; i += 2) {

      Vec2i   world_tile_coords = vec2i(i / 2, j);
      TilePos tilepos           = tile_pos_from_world_tile_space(world_tile_coords);
      Tile*   tile              = tile_ensure_get(level, tilepos);

      tile->placement_reason = TilePlacementReason_Loaded;

      if (plan_line[i] != ' ') {
        i32 tile_x = i / 2;
        i32 tile_y = j;

        sprite_count++;

        tile->type   = TileType_Floor;
        tile->sprite = floor_sprite(sprite_count);

        Entity* entity;

        switch (plan_line[i]) {
          // clang-format off

          // F == floor, W == wall

          // walls
          //   2
          // 4 F 6
          //   8

        case '2': tile->type = TileType_Void; tile->sprite = S_WallTop; break;
        case '4': tile->type = TileType_Void; tile->sprite = S_WallLeft; break;
        case '6': tile->type = TileType_Void; tile->sprite = S_WallRight; break;
        case '8': tile->type = TileType_Void; tile->sprite = S_WallBottom; break;

          // outside corners
          // 1 W 3
          // W F W
          // 7 W 9

        case '1': tile->type = TileType_Void; tile->sprite = S_CornerA_TL; break;
        case '3': tile->type = TileType_Void; tile->sprite = S_CornerA_TR; break;
        case '7': tile->type = TileType_Void; tile->sprite = S_CornerA_BL; break;
        case '9': tile->type = TileType_Void; tile->sprite = S_CornerA_BR; break;

          // inside corners
          // w F x
          // F F F
          // y F z

        case 'w': tile->type = TileType_Void; tile->sprite = S_CornerB_TL; break;
        case 'x': tile->type = TileType_Void; tile->sprite = S_CornerB_TR; break;
        case 'y': tile->type = TileType_Void; tile->sprite = S_CornerB_BL; break;
        case 'z': tile->type = TileType_Void; tile->sprite = S_CornerB_BR; break;

        // clang-format on
        case 'H':
          have_hero = true;

          entity = &(level->entities[0]);
          entity_defaults(entity, EntityRole_Hero);
          entity_place(level, entity, tile_x, tile_y, 0.0f);
          break;
        case 'B':
          RONA_ASSERT(next_non_hero_entity_index < level->max_num_entities);

          entity = &(level->entities[next_non_hero_entity_index++]);
          entity_defaults(entity, EntityRole_Block);
          entity_place(level, entity, tile_x, tile_y, 0.5f);
          break;
        case 'U':
          RONA_ASSERT(next_non_hero_entity_index < level->max_num_entities);

          entity = &(level->entities[next_non_hero_entity_index++]);
          entity_defaults(entity, EntityRole_Pit);
          entity_place(level, entity, tile_x, tile_y, 1.0f);
          break;
        case 's':
          RONA_ASSERT(next_non_hero_entity_index < level->max_num_entities);

          entity = &(level->entities[next_non_hero_entity_index++]);
          entity_defaults(entity, EntityRole_LevelExit);
          entity_place(level, entity, tile_x, tile_y, 1.0f);
          break;
        }

      } else {
        tile->type   = TileType_Void;
        tile->sprite = S_Blank;
      }
    }
  }

  if (!have_hero) {
    rona_error("There's no hero on the level");
  }
}

void entity_defaults(Entity* entity, EntityRole entity_role) {
  const f32 max_speed = 9.0f;

  entity->handle                  = g_entity_handle++;
  entity->entity_role             = entity_role;
  entity->no_further_entities     = false;
  entity->ignore                  = false;
  entity->entity_state            = EntityState_Standing;
  entity->is_animated             = true;
  entity->entity_facing           = EntityFacing_Right;
  entity->entity_animation        = EntityAnimation_Idle;
  entity->animation_speed         = 1.0f;
  entity->animation_frame         = 0;
  entity->animation_frame_counter = 0;
  entity->world_max_speed         = max_speed;

  switch (entity_role) {
  case EntityRole_Hero:
    entity->z_order                   = 3;
    entity->animated_character_sprite = ACS_BlueKnight;
    break;
  case EntityRole_Enemy:
    break;
  case EntityRole_Block:
    entity->z_order     = 2;
    entity->is_animated = false;
    break;
  case EntityRole_Pit:
    entity->z_order     = 1;
    entity->is_animated = false;
    break;
  case EntityRole_FilledPit:
    entity->z_order     = 1;
    entity->is_animated = false;
    break;
  case EntityRole_LevelExit:
    entity->z_order     = 1;
    entity->is_animated = false;
    break;
  };
}

Entity* entity_from_handle(Level* level, Handle handle) {
  for (i32 i = 0; i < level->max_num_entities; i++) {
    if (level->entities[i].handle == handle) {
      return &(level->entities[i]);
    }
  }
  return NULL;
}
