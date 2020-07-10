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

#define MAX_OCCUPANTS_ALLOWED 10

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

bool try_moving_block(Level* level, Entity* block, Direction direction) {
  Vec2i new_pos = vec2i_add_direction(&block->board_pos, direction);

  if (new_pos.x < 0 || new_pos.x >= level->width) {
    return false;
  }
  if (new_pos.y < 0 || new_pos.y >= level->height) {
    return false;
  }

  i32 new_tile_index = new_pos.x + (new_pos.y * level->width);
  if (level->tiles[new_tile_index].tile_type == TileType_Void) {
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
    Command* command = command_add(level);

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

    command_execute(command, CommandExecute_Play);
  }

  return true;
}

bool try_moving_hero(Level* level, Entity* hero, Direction direction) {
  Vec2i new_pos = vec2i_add_direction(&hero->board_pos, direction);

  if (new_pos.x < 0 || new_pos.x >= level->width) {
    return false;
  }
  if (new_pos.y < 0 || new_pos.y >= level->height) {
    return false;
  }

  i32 new_tile_index = new_pos.x + (new_pos.y * level->width);
  if (level->tiles[new_tile_index].tile_type == TileType_Void) {
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
      bool block_can_move = try_moving_block(level, block, direction);
      if (block_can_move) {
        // happy path - move onto updating the hero position
      } else {
        return false;
      }
    }
  }

  {
    Command* command = command_add(level);

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

    command_execute(command, CommandExecute_Play);
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
  level->max_num_entities = 10;
  level->entities = (Entity*)BUMP_ALLOC(&(level->mem), sizeof(Entity) * level->max_num_entities);
  for (i32 i = 0; i < level->max_num_entities; i++) {
    level->entities[i].exists = false;
  }

  bool have_hero = false;
  i32  next_non_hero_entity_index = 1;

  // centre the level in the stage
  level->offset_stage_from_world =
      vec2(((f32)STAGE_WIDTH - (((f32)dbl_width / 2.0f) * TILE_WIDTH)) / 2.0f,
           ((f32)height * TILE_HEIGHT) / 2.0f);

  level->mesh_floor = (Mesh*)BUMP_ALLOC(&(level->mem), sizeof(Mesh));

  i32 width = dbl_width / 2;
  level->width = width;
  level->height = height;
  i32 num_tiles = level->width * level->height;
  level->tiles = (Tile*)BUMP_ALLOC(&(level->mem), sizeof(Tile) * num_tiles);

  const f32 max_speed = 9.0f;

  for (i32 j = 0; j < height; j++) {

    char* plan_line = layout[j];

    for (i32 i = 0; i < dbl_width; i += 2) {
      i32 tile_index = (i / 2) + (j * width);
      if (plan_line[i] != ' ') {
        i32 tile_x = i / 2;
        i32 tile_y = j;

        level->tiles[tile_index].tile_type = TileType_Floor;

        if (plan_line[i] == 'H') { // hero
          Entity* hero = &(level->entities[0]);

          have_hero = true;
          hero->exists = true;
          hero->entity_type = EntityType_Hero;
          hero->entity_state = EntityState_Standing;
          hero->mesh = game_state->mesh_hero;
          hero->world_max_speed = max_speed;
          entity_place(level, hero, tile_x, tile_y, 0.0f);
          entity_colour_as_hsluv(hero, 290.0f, 90.0f, 30.0f);

        } else if (plan_line[i] == 'B') { // block
          RONA_ASSERT(next_non_hero_entity_index < level->max_num_entities);

          Entity* block = &(level->entities[next_non_hero_entity_index++]);

          block->exists = true;
          block->entity_type = EntityType_Block;
          block->entity_state = EntityState_Standing;
          block->mesh = game_state->mesh_block;
          block->world_max_speed = max_speed;
          entity_place(level, block, tile_x, tile_y, 0.5f);
          entity_colour_as_hsluv(block, 10.0f, 80.0f, 20.0f);

        } else if (plan_line[i] == 'U') { // pit
          RONA_ASSERT(next_non_hero_entity_index < level->max_num_entities);

          Entity* pit = &(level->entities[next_non_hero_entity_index++]);

          pit->exists = true;
          pit->entity_type = EntityType_Pit;
          pit->entity_state = EntityState_Standing;
          pit->mesh = game_state->mesh_pit;
          pit->world_max_speed = max_speed;
          entity_place(level, pit, tile_x, tile_y, 1.0f);
          entity_colour_as_hsluv(pit, 10.0f, 80.0f, 2.0f);
        }
      } else {
        level->tiles[tile_index].tile_type = TileType_Void;
      }
    }
  }

  if (!have_hero) {
    RONA_ERROR("There's no hero on the level\n");
  }
}

void mesh_floor_lib_load(Level* level, RonaGL* gl, BumpAllocator* transient, Tileset* tileset) {

  Vec4 fg, bg;

  Colour ground_colour_fg;
  colour_from(&ground_colour_fg, ColourFormat_RGB, ColourFormat_HSLuv, 60.0f, 80.0f, 90.0f, 1.0f);

  Colour ground_colour_bg;
  colour_from(&ground_colour_bg, ColourFormat_RGB, ColourFormat_HSLuv, 260.0f, 80.0f, 50.0f, 1.0f);

  vec4_from_colour(&fg, &ground_colour_fg);
  vec4_from_colour(&bg, &ground_colour_bg);

  Mesh* mesh = level->mesh_floor;

  gl->genVertexArrays(1, &mesh->vao); // Vertex Array Object
  gl->bindVertexArray(mesh->vao);

  Vec2 sprite = tileset_get_uv(tileset, TS_Debug4Corners);
  f32  u = sprite.u;
  f32  v = sprite.v;
  f32  ud = tileset->uv_unit.u;
  f32  vd = tileset->uv_unit.v;

  f32 half_dim_x = TILE_WIDTH * 0.5f;
  f32 half_dim_y = TILE_HEIGHT * 0.5f;

  // count the number of floor tiles to generate
  //
  i32 num_floor_tiles = 0;
  for (i32 i = 0; i < level->width * level->height; i++) {
    if (level->tiles[i].tile_type == TileType_Floor) {
      num_floor_tiles++;
    }
  }

  mesh->num_elements = 6 * num_floor_tiles;
  i32 stride = TILED_QUAD_GEOMETRY_SIZEOF_1;
  i32 num_vert_elements = stride * num_floor_tiles;

  u32  sizeof_vertices = sizeof(f32) * num_vert_elements;
  u32  sizeof_indices = sizeof(u32) * mesh->num_elements;
  f32* vertices = (f32*)BUMP_ALLOC(transient, sizeof_vertices);
  u32* indices = (u32*)BUMP_ALLOC(transient, sizeof_indices);

  // build geometry
  i32 tile_count = 0;

  // clang-format off
  f32 *e;
  for (i32 j = 0; j < level->height; j++) {
    for (i32 i = 0; i < level->width; i++) {
      if (level->tiles[i + (j * level->width)].tile_type == TileType_Floor) {
        i32 e_index = tile_count * stride;
        e = &vertices[e_index];

        *e++ = -half_dim_x + ((f32)i * TILE_WIDTH) + HALF_TILE_WIDTH;
        *e++ = -half_dim_y + ((f32)j * TILE_HEIGHT) + HALF_TILE_HEIGHT;

        *e++ = u;
        *e++ = v;

        *e++ = fg.e[0]; *e++ = fg.e[1]; *e++ = fg.e[2]; *e++ = fg.e[3];
        *e++ = bg.e[0]; *e++ = bg.e[1]; *e++ = bg.e[2]; *e++ = bg.e[3];

        *e++ = -half_dim_x + ((f32)i * TILE_WIDTH) + HALF_TILE_WIDTH;
        *e++ =  half_dim_y + ((f32)j * TILE_HEIGHT) + HALF_TILE_HEIGHT;

        *e++ = u;
        *e++ = v + vd;

        *e++ = fg.e[0]; *e++ = fg.e[1]; *e++ = fg.e[2]; *e++ = fg.e[3];
        *e++ = bg.e[0]; *e++ = bg.e[1]; *e++ = bg.e[2]; *e++ = bg.e[3];

        *e++ = half_dim_x + ((f32)i * TILE_WIDTH) + HALF_TILE_WIDTH;
        *e++ = half_dim_y + ((f32)j * TILE_HEIGHT) + HALF_TILE_HEIGHT;

        *e++ = u + ud;
        *e++ = v + vd;

        *e++ = fg.e[0]; *e++ = fg.e[1]; *e++ = fg.e[2]; *e++ = fg.e[3];
        *e++ = bg.e[0]; *e++ = bg.e[1]; *e++ = bg.e[2]; *e++ = bg.e[3];

        *e++ = half_dim_x + ((f32)i * TILE_WIDTH) + HALF_TILE_WIDTH;
        *e++ = -half_dim_y + ((f32)j * TILE_HEIGHT) + HALF_TILE_HEIGHT;

        *e++ = u + ud;
        *e++ = v;

        *e++ = fg.e[0]; *e++ = fg.e[1]; *e++ = fg.e[2]; *e++ = fg.e[3];
        *e++ = bg.e[0]; *e++ = bg.e[1]; *e++ = bg.e[2]; *e++ = bg.e[3];

        i32 i_index = tile_count * 6;
        i32 offset = tile_count * 4;
        indices[i_index + 0] = 0 + offset;
        indices[i_index + 1] = 1 + offset;
        indices[i_index + 2] = 2 + offset;
        indices[i_index + 3] = 0 + offset;
        indices[i_index + 4] = 2 + offset;
        indices[i_index + 5] = 3 + offset;

        tile_count++;
      }
    }
  }
  // clang-format on

  // the type of a Vertex Buffer Object is GL_ARRAY_BUFFER
  //
  GLuint vbo;
  gl->genBuffers(1, &vbo);
  gl->bindBuffer(GL_ARRAY_BUFFER, vbo);
  gl->bufferData(GL_ARRAY_BUFFER, sizeof_vertices, vertices,
                 GL_STATIC_DRAW); // the data is set only once and used many times.
  gl->bindBuffer(GL_ARRAY_BUFFER, 0);

  GLuint ebo;
  gl->genBuffers(1, &ebo);
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  gl->bufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof_indices, indices, GL_STATIC_DRAW);
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  gl->bindBuffer(GL_ARRAY_BUFFER, vbo);
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

  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  gl->bindVertexArray(0);
}

void mesh_floor_lib_unload(Level* level, RonaGL* gl) {
  gl->deleteVertexArrays(1, &level->mesh_floor->vao);
}
