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

#define MAX_OCCUPANTS_ALLOWED 10

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
  tile_regenerate_geometry(level, gl, render_struct);
}

void level_lib_unload(Level* level, RonaGL* gl) {
  graphic_teardown(&level->chunks_graphic, gl);
  graphic_teardown(&level->entities_graphic, gl);
}

// how many entities are there on the level at the given position?
// returns the amount and fills in the entities array
i32 enitites_at_board_position(Entity** occupants, i32 max_allowed, Level* level, Vec2i* pos) {
  i32 num_occupants = 0;
  for (i32 i = 0; i < level->max_num_entities; i++) {
    if (level->entities[i].exists == false) {
      return num_occupants;
    }

    Entity* e = &(level->entities[i]);
    if (e->board_pos.x == pos->x && e->board_pos.y == pos->y) {
      RONA_ASSERT(num_occupants < max_allowed);

      occupants[num_occupants] = e;
      num_occupants++;
    }
  }

  return num_occupants;
}

Vec2i vec2i_add_direction(Vec2i* pos, Direction direction) {
  Vec2i res = vec2i_clone(pos);
  switch (direction) {
  case Direction_North:
    res.y -= 1;
    break;
  case Direction_South:
    res.y += 1;
    break;
  case Direction_East:
    res.x += 1;
    break;
  case Direction_West:
    res.x -= 1;
    break;
  };

  return res;
}

void world_from_board(Vec3* dst, i32 x, i32 y, f32 z) {
  vec3_set(dst, ((f32)x * TILE_WIDTH) + HALF_TILE_WIDTH, ((f32)y * TILE_HEIGHT) + HALF_TILE_HEIGHT, z);
}

bool try_moving_block(Level* level, Entity* block, Direction direction, GameState* game_state) {
  Vec2i new_pos = vec2i_add_direction(&block->board_pos, direction);

  Tile* tile = tile_from_world_tile_space(level, new_pos);
  if (tile->type == TileType_Void) {
    return false;
  }

  // check if this will push any other entities
  Entity* occupants[MAX_OCCUPANTS_ALLOWED];
  i32     num_occupants = enitites_at_board_position(occupants, MAX_OCCUPANTS_ALLOWED, level, &new_pos);
  if (num_occupants > 0) {

    bool is_occupier_block = false;
    bool is_occupier_pit   = false;

    for (i32 i = 0; i < num_occupants; i++) {
      if (occupants[i]->entity_role == EntityRole_Block) {
        is_occupier_block = true;
      }
      if (occupants[i]->entity_role == EntityRole_Pit) {
        is_occupier_pit = true;
      }
    }

    if (is_occupier_pit && is_occupier_block) {
      // theres a pit covered with a block. this can be walked upon
      // happy path - move onto updating the block position
    } else if (is_occupier_pit) {
      // just a pit here
      // happy path - move onto updating the block position to fall into the pit
    } else if (is_occupier_block) {
      return false;
    }
  }

  {
    Command* command = command_add(&level->undo_redo, &level->fixed_block_allocator, game_state);

    command->type   = CommandType_EntityMove;
    command->entity = block;

    CommandParamsEntityMove* old_params = &command->params.entity_move.old_params;
    CommandParamsEntityMove* new_params = &command->params.entity_move.new_params;

    old_params->board_pos = block->board_pos;
    new_params->board_pos = new_pos;

    old_params->world_target = block->world_target;
    world_from_board(&new_params->world_target, new_pos.x, new_pos.y, block->world_target.z);

    old_params->world_pos = block->world_pos;
    new_params->world_pos = new_params->world_target;

    old_params->entity_state = block->entity_state;
    new_params->entity_state = EntityState_Moving;

    command_execute(command, CommandExecute_Play, game_state);
  }

  return true;
}

bool try_moving_hero(Level* level, Entity* hero, Direction direction, GameState* game_state) {
  Vec2i new_pos = vec2i_add_direction(&hero->board_pos, direction);

  Tile* tile = tile_from_world_tile_space(level, new_pos);
  if (tile->type == TileType_Void) {
    return false;
  }

  // check if this will push any other entities
  Entity* occupants[MAX_OCCUPANTS_ALLOWED];
  i32     num_occupants = enitites_at_board_position(occupants, MAX_OCCUPANTS_ALLOWED, level, &new_pos);
  if (num_occupants > 0) {
    bool    is_occupier_block = false;
    bool    is_occupier_pit   = false;
    Entity* block;
    for (i32 i = 0; i < num_occupants; i++) {
      if (occupants[i]->entity_role == EntityRole_Block) {
        block             = occupants[i];
        is_occupier_block = true;
      }
      if (occupants[i]->entity_role == EntityRole_Pit) {
        is_occupier_pit = true;
      }
    }

    if (is_occupier_pit && is_occupier_block) {
      // theres a pit covered with a block. this can be walked upon
      // happy path - move onto updating the hero position
    } else if (is_occupier_pit) {
      return false;
    } else if (is_occupier_block) {
      // check if the block can be pushed along
      bool block_can_move = try_moving_block(level, block, direction, game_state);
      if (block_can_move) {
        // happy path - move onto updating the hero position
      } else {
        return false;
      }
    }
  }

  {
    Command* command = command_add(&level->undo_redo, &level->fixed_block_allocator, game_state);

    command->type   = CommandType_EntityMove;
    command->entity = hero;

    CommandParamsEntityMove* old_params = &command->params.entity_move.old_params;
    CommandParamsEntityMove* new_params = &command->params.entity_move.new_params;

    old_params->board_pos = hero->board_pos;
    new_params->board_pos = new_pos;

    old_params->world_target = hero->world_target;
    world_from_board(&new_params->world_target, new_pos.x, new_pos.y, hero->world_target.z);

    old_params->world_pos = hero->world_pos;
    new_params->world_pos = new_params->world_target;

    old_params->entity_state = hero->entity_state;
    new_params->entity_state = EntityState_Moving;

    command_execute(command, CommandExecute_Play, game_state);
  }

  return true;
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
    level->entities[i].exists = false;
  }

  bool have_hero                  = false;
  i32  next_non_hero_entity_index = 1;

  i32 sprite_count = 0;

  const f32 max_speed = 9.0f;

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

        Entity *hero, *block, *pit;

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

        case 's': tile->type = TileType_Void; tile->sprite = S_LevelExit; break;

        // clang-format on
        case 'H':
          hero = &(level->entities[0]);

          have_hero                       = true;
          hero->exists                    = true;
          hero->z_order                   = 3;
          hero->entity_role               = EntityRole_Hero;
          hero->entity_state              = EntityState_Standing;
          hero->is_animated               = true;
          hero->entity_facing             = EntityFacing_Right;
          hero->animated_character_sprite = ACS_BlueKnight;
          hero->entity_animation          = EntityAnimation_Idle;
          hero->animation_speed           = 1.0f;
          hero->animation_frame           = 0;
          hero->animation_frame_counter   = 0;
          // hero->graphic = &(game_state->graphic_hero);
          hero->world_max_speed = max_speed;
          entity_place(level, hero, tile_x, tile_y, 0.0f);
          break;
        case 'B':
          RONA_ASSERT(next_non_hero_entity_index < level->max_num_entities);

          block = &(level->entities[next_non_hero_entity_index++]);

          block->exists        = true;
          block->z_order       = 2;
          block->entity_role   = EntityRole_Block;
          block->entity_state  = EntityState_Standing;
          block->entity_facing = EntityFacing_Right;
          block->is_animated   = false;
          // block->graphic = &(game_state->graphic_block);
          block->world_max_speed = max_speed;
          entity_place(level, block, tile_x, tile_y, 0.5f);
          break;
        case 'U':
          RONA_ASSERT(next_non_hero_entity_index < level->max_num_entities);

          pit = &(level->entities[next_non_hero_entity_index++]);

          pit->exists        = true;
          pit->z_order       = 1;
          pit->entity_role   = EntityRole_Pit;
          pit->entity_state  = EntityState_Standing;
          pit->is_animated   = false;
          pit->entity_facing = EntityFacing_Right;
          // pit->graphic = &(game_state->graphic_pit);
          pit->world_max_speed = max_speed;
          entity_place(level, pit, tile_x, tile_y, 1.0f);
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
