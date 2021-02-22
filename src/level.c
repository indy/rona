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
  chunk_startup(level);
}

void level_shutdown(Level* level) {
  chunk_shutdown(level);
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
  vec3_set(dst, ((f32)x * TILE_WIDTH) + HALF_TILE_WIDTH, ((f32)y * TILE_HEIGHT) + HALF_TILE_HEIGHT,
           z);
}

bool try_moving_block(Level* level, Entity* block, Direction direction, GameState* game_state) {
  Vec2i new_pos = vec2i_add_direction(&block->board_pos, direction);

  Tile* tile = chunk_tile_from_world_tile_space(level, new_pos);
  if (tile->type == TileType_Void) {
    return false;
  }

  // check if this will push any other entities
  Entity* occupants[MAX_OCCUPANTS_ALLOWED];
  i32 num_occupants = enitites_at_board_position(occupants, MAX_OCCUPANTS_ALLOWED, level, &new_pos);
  if (num_occupants > 0) {

    bool is_occupier_block = false;
    bool is_occupier_pit = false;

    for (i32 i = 0; i < num_occupants; i++) {
      if (occupants[i]->entity_type == EntityType_Block) {
        is_occupier_block = true;
      }
      if (occupants[i]->entity_type == EntityType_Pit) {
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
    Command* command = command_add(&level->bump_allocator, &level->undo_redo, game_state);

    command->type = CommandType_EntityMove;
    command->entity = block;

    CommandParamsEntityMove* old_params = &command->data.entity_move.old_params;
    CommandParamsEntityMove* new_params = &command->data.entity_move.new_params;

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

  Tile* tile = chunk_tile_from_world_tile_space(level, new_pos);
  if (tile->type == TileType_Void) {
    return false;
  }

  // check if this will push any other entities
  Entity* occupants[MAX_OCCUPANTS_ALLOWED];
  i32 num_occupants = enitites_at_board_position(occupants, MAX_OCCUPANTS_ALLOWED, level, &new_pos);
  if (num_occupants > 0) {
    bool    is_occupier_block = false;
    bool    is_occupier_pit = false;
    Entity* block;
    for (i32 i = 0; i < num_occupants; i++) {
      if (occupants[i]->entity_type == EntityType_Block) {
        block = occupants[i];
        is_occupier_block = true;
      }
      if (occupants[i]->entity_type == EntityType_Pit) {
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
    Command* command = command_add(&level->bump_allocator, &level->undo_redo, game_state);

    command->type = CommandType_EntityMove;
    command->entity = hero;

    CommandParamsEntityMove* old_params = &command->data.entity_move.old_params;
    CommandParamsEntityMove* new_params = &command->data.entity_move.new_params;

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

void entity_colour_as_hsluv(Entity* entity, f32 h, f32 s, f32 l) {
  Colour c;
  colour_from(&c, ColourFormat_RGB, ColourFormat_HSLuv, h, s, l, 1.0f);
  vec4_from_colour(&entity->colour, &c);
}

void level_build(GameState* game_state, Level* level, i32 dbl_width, i32 height,
                 char layout[][dbl_width]) {

  level->sb_chunks = NULL;

  level->max_num_entities = 10;
  level->entities =
      (Entity*)BUMP_ALLOC(&(level->bump_allocator), sizeof(Entity) * level->max_num_entities);
  for (i32 i = 0; i < level->max_num_entities; i++) {
    level->entities[i].exists = false;
  }

  bool have_hero = false;
  i32  next_non_hero_entity_index = 1;

  const f32 max_speed = 9.0f;

  for (i32 j = 0; j < height; j++) {

    char* plan_line = layout[j];

    for (i32 i = 0; i < dbl_width; i += 2) {

      Vec2i    world_tile_coords = vec2i(i / 2, j);
      ChunkPos chunkpos = chunk_pos_from_world_tile_space(world_tile_coords);
      Tile*    tile = chunk_tile_ensure_get(level, chunkpos);

      if (plan_line[i] != ' ') {
        i32 tile_x = i / 2;
        i32 tile_y = j;

        tile->type = TileType_Floor;
        tile->sprite = TS_Debug4Corners;

        Entity *hero, *block, *pit;

        switch (plan_line[i]) {
        // clang-format off
        case '0': tile->type = TileType_Void; tile->sprite = TS_WallHorizontal; break;
        case '1': tile->type = TileType_Void; tile->sprite = TS_WallVertical; break;
        case '2': tile->type = TileType_Void; tile->sprite = TS_WallHorizontalPoint; break;
        case '3': tile->type = TileType_Void; tile->sprite = TS_WallTLCorner; break;
        case '4': tile->type = TileType_Void; tile->sprite = TS_WallTRCorner; break;
        case '5': tile->type = TileType_Void; tile->sprite = TS_WallBLCorner; break;
        case '6': tile->type = TileType_Void; tile->sprite = TS_WallBRCorner; break;
        case '7': tile->type = TileType_Void; tile->sprite = TS_WallTeeLeft; break;
        case '8': tile->type = TileType_Void; tile->sprite = TS_WallTeeRight; break;
        case '9': tile->type = TileType_Void; tile->sprite = TS_WallTeeDown; break;
        case 'a': tile->type = TileType_Void; tile->sprite = TS_WallTeeUp; break;
        case 'b': tile->type = TileType_Void; tile->sprite = TS_WallCross; break;
        case 's': tile->type = TileType_Void; tile->sprite = TS_DoorClosed; break;
        // clang-format on
        case 'H':
          hero = &(level->entities[0]);

          have_hero = true;
          hero->exists = true;
          hero->entity_type = EntityType_Hero;
          hero->entity_state = EntityState_Standing;
          hero->graphic = &(game_state->graphic_hero);
          hero->world_max_speed = max_speed;
          entity_place(level, hero, tile_x, tile_y, 0.0f);
          entity_colour_as_hsluv(hero, 290.0f, 90.0f, 30.0f);
          break;
        case 'B':
          RONA_ASSERT(next_non_hero_entity_index < level->max_num_entities);

          block = &(level->entities[next_non_hero_entity_index++]);

          block->exists = true;
          block->entity_type = EntityType_Block;
          block->entity_state = EntityState_Standing;
          block->graphic = &(game_state->graphic_block);
          block->world_max_speed = max_speed;
          entity_place(level, block, tile_x, tile_y, 0.5f);
          entity_colour_as_hsluv(block, 10.0f, 80.0f, 20.0f);
          break;
        case 'U':
          RONA_ASSERT(next_non_hero_entity_index < level->max_num_entities);

          pit = &(level->entities[next_non_hero_entity_index++]);

          pit->exists = true;
          pit->entity_type = EntityType_Pit;
          pit->entity_state = EntityState_Standing;
          pit->graphic = &(game_state->graphic_pit);
          pit->world_max_speed = max_speed;
          entity_place(level, pit, tile_x, tile_y, 1.0f);
          entity_colour_as_hsluv(pit, 10.0f, 80.0f, 2.0f);
          break;
        }

      } else {
        tile->type = TileType_Void;
        tile->sprite = TS_DebugBlank;
      }
    }
  }

  if (!have_hero) {
    RONA_ERROR("There's no hero on the level\n");
  }
}
