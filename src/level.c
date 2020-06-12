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


// how many entities are there on the level at the given position?
// returns the amount and fills in the entities array
i32 enitites_at_board_position(Entity **occupants, i32 max_allowed, Level* level, Vec2i *pos) {
  i32 num_occupants = 0;
  for (i32 i = 0; i < level->max_num_entities; i++) {
    if (level->entities[i].exists == false) {
      return num_occupants;
    }

    Entity *e = &(level->entities[i]);
    if (e->board_pos.x == pos->x && e->board_pos.y == pos->y) {
      RONA_ASSERT(num_occupants < max_allowed);

      occupants[num_occupants] = e;
      num_occupants++;
    }
  }

  return num_occupants;
}

void vec2i_add_direction(Vec2i *pos, Direction direction) {
  switch(direction) {
  case Direction_North: pos->y += 1; break;
  case Direction_South: pos->y -= 1; break;
  case Direction_East: pos->x += 1; break;
  case Direction_West: pos->x -= 1; break;
  };
}

bool try_moving_block(Level *level, Entity *block, Direction direction) {
  Vec2i new_pos;
  new_pos.x = block->board_pos.x;
  new_pos.y = block->board_pos.y;

  vec2i_add_direction(&new_pos, direction);

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
#define MAX_OCCUPANTS_ALLOWED 10
  Entity *occupants[MAX_OCCUPANTS_ALLOWED];
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

  block->board_pos.x = new_pos.x;
  block->board_pos.y = new_pos.y;
  block->world_target.x = (f32)block->board_pos.x;
  block->world_target.y = (f32)block->board_pos.y;
  block->entity_state = EntityState_Moving;

  return true;
}

bool try_moving_hero(Level *level, Entity *hero, Direction direction) {
  Vec2i new_pos;
  new_pos.x = hero->board_pos.x;
  new_pos.y = hero->board_pos.y;

  vec2i_add_direction(&new_pos, direction);

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
#define MAX_OCCUPANTS_ALLOWED 10
  Entity *occupants[MAX_OCCUPANTS_ALLOWED];
  i32 num_occupants = enitites_at_board_position(occupants, MAX_OCCUPANTS_ALLOWED, level, &new_pos);
  if (num_occupants > 0) {
    bool is_occupier_block = false;
    bool is_occupier_pit = false;
    Entity *block;
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

  hero->board_pos.x = new_pos.x;
  hero->board_pos.y = new_pos.y;
  hero->world_target.x = (f32)hero->board_pos.x;
  hero->world_target.y = (f32)hero->board_pos.y;
  hero->entity_state = EntityState_Moving;

  return true;
}





// place an entity at the given board positions
void entity_place(Level *level, Entity *entity, i32 board_pos_x, i32 board_pos_y) {
  entity->board_pos.x = board_pos_x;
  entity->board_pos.y = board_pos_y;
  entity->world_pos.x = (f32)board_pos_x;
  entity->world_pos.y = (f32)board_pos_y;
  entity->world_target.x = (f32)board_pos_x;
  entity->world_target.y = (f32)board_pos_y;
}

void entity_colour_as_hsluv(Entity *entity, f32 h, f32 s, f32 l) {
  Colour c;
  colour_from(&c, ColourFormat_sRGB, ColourFormat_HSLuv, h, s, l, 1.0f);
  entity->colour.r = c.element[0];
  entity->colour.g = c.element[1];
  entity->colour.b = c.element[2];
  entity->colour.a = c.element[3];
}

void level_build(GameState *game_state, Level *level, i32 dbl_width, i32 height, char layout[][dbl_width]) {
  level->max_num_entities = 10;
  level->entities = (Entity *)ARENA_ALLOC(&(level->mem), sizeof(Entity) * level->max_num_entities);
  for (i32 i = 0; i < level->max_num_entities; i++) {
    level->entities[i].exists = false;
  }

  bool have_hero = false;
  i32 next_non_hero_entity_index = 1;

  level->mesh_floor = (Mesh *)ARENA_ALLOC(&(level->mem), sizeof(Mesh));

  i32 width = dbl_width / 2;
  level->width = width;
  level->height = height;
  i32 num_tiles = level->width * level->height;
  level->tiles = (Tile *)ARENA_ALLOC(&(level->mem), sizeof(Tile) * num_tiles);

  for (i32 j = 0; j < height; j++) {

    char *plan_line = layout[height - 1 - j];

    for (i32 i = 0; i < dbl_width; i+=2) {
      i32 tile_index = (i / 2) + (j * width);
      if (plan_line[i] != ' ') {
        i32 tile_x = i / 2;
        i32 tile_y = j;

        level->tiles[tile_index].tile_type = TileType_Floor;

        if (plan_line[i] == 'H') { // hero
          Entity *hero = &(level->entities[0]);

          have_hero = true;
          hero->exists = true;
          hero->entity_type = EntityType_Hero;
          hero->entity_state = EntityState_Standing;
          hero->mesh = game_state->mesh_hero;
          hero->world_max_speed = 18.0f;
          entity_place(level, hero, tile_x, tile_y);
          entity_colour_as_hsluv(hero, 10.0f, 90.0f, 50.0f);

        } else if (plan_line[i] == 'B') { // block
          RONA_ASSERT(next_non_hero_entity_index < level->max_num_entities);

          Entity *block = &(level->entities[next_non_hero_entity_index++]);

          block->exists = true;
          block->entity_type = EntityType_Block;
          block->entity_state = EntityState_Standing;
          block->mesh = game_state->mesh_block;
          block->world_max_speed = 18.0f;
          entity_place(level, block, tile_x, tile_y);
          entity_colour_as_hsluv(block, 10.0f, 80.0f, 20.0f);

        } else if (plan_line[i] == 'U') { // pit
          RONA_ASSERT(next_non_hero_entity_index < level->max_num_entities);

          Entity *pit = &(level->entities[next_non_hero_entity_index++]);

          pit->exists = true;
          pit->entity_type = EntityType_Pit;
          pit->entity_state = EntityState_Standing;
          pit->mesh = game_state->mesh_pit;
          pit->world_max_speed = 18.0f;
          entity_place(level, pit, tile_x, tile_y);
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

void mesh_floor_lib_load(Level *level, RonaGl *gl, MemoryArena *transient) {
  Mesh *mesh = level->mesh_floor;

  gl->genVertexArrays(1, &mesh->vao); // Vertex Array Object
  gl->bindVertexArray(mesh->vao);

  #include "../target/shader.vert.c"
  SHADER_AS_STRING(transient, vertexSource, shader_vert);

  #include "../target/shader.frag.c"
  SHADER_AS_STRING(transient, fragmentSource, shader_frag);

  mesh->shader_program = create_shader_program(gl, vertexSource, fragmentSource);

  f32 half_dim = 0.48f;

  // count the number of floor tiles to generate
  //
  i32 num_floor_tiles = 0;
  for(i32 i = 0; i < level->width * level->height; i++) {
    if (level->tiles[i].tile_type == TileType_Floor) {
      num_floor_tiles++;
    }
  }

  mesh->num_elements = 6 * num_floor_tiles;
  i32 num_vert_elements = 8 * num_floor_tiles;

  u32 sizeof_vertices = sizeof(f32) * num_vert_elements;
  u32 sizeof_indices = sizeof(u32) * mesh->num_elements;
  f32 *vertices = (f32 *)ARENA_ALLOC(transient, sizeof_vertices);
  u32 *indices = (u32 *)ARENA_ALLOC(transient, sizeof_indices);

  // build geometry
  i32 tile_count = 0;

  for(i32 j = 0; j < level->height; j++) {
    for(i32 i = 0; i < level->width; i++) {
      if (level->tiles[i + (j*level->width)].tile_type == TileType_Floor) {
        i32 v_index = tile_count * 8;
        vertices[v_index + 0] = -half_dim + (f32)i; vertices[v_index + 1] =  half_dim + (f32)j;
        vertices[v_index + 2] = -half_dim + (f32)i; vertices[v_index + 3] = -half_dim + (f32)j;
        vertices[v_index + 4] =  half_dim + (f32)i; vertices[v_index + 5] = -half_dim + (f32)j;
        vertices[v_index + 6] =  half_dim + (f32)i; vertices[v_index + 7] =  half_dim + (f32)j;

        i32 i_index = tile_count * 6;
        i32 offset = tile_count * 4;
        indices[i_index + 0] = 0 + offset; indices[i_index + 1] = 1 + offset; indices[i_index + 2] = 2 + offset;
        indices[i_index + 3] = 0 + offset; indices[i_index + 4] = 2 + offset; indices[i_index + 5] = 3 + offset;

        tile_count++;
      }
    }
  }

  // the type of a Vertex Buffer Object is GL_ARRAY_BUFFER
  //
  GLuint vbo;
  gl->genBuffers(1, &vbo);
  gl->bindBuffer(GL_ARRAY_BUFFER, vbo);
  gl->bufferData(GL_ARRAY_BUFFER, sizeof_vertices, vertices, GL_STATIC_DRAW); // the data is set only once and used many times.
  gl->bindBuffer(GL_ARRAY_BUFFER, 0);

  GLuint ebo;
  gl->genBuffers(1, &ebo);
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  gl->bufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof_indices, indices, GL_STATIC_DRAW);
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  gl->useProgram(mesh->shader_program);

  mesh->uniform_colour = gl->getUniformLocation(mesh->shader_program, "colour");
  mesh->uniform_proj_matrix = gl->getUniformLocation(mesh->shader_program, "proj_matrix");
  mesh->uniform_pos = gl->getUniformLocation(mesh->shader_program, "pos");

  gl->bindBuffer(GL_ARRAY_BUFFER, vbo);
  gl->enableVertexAttribArray(0);
  gl->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);

  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  gl->bindVertexArray(0);
}

void mesh_floor_lib_unload(Level *level, RonaGl *gl) {
  gl->deleteVertexArrays(1, &level->mesh_floor->vao);
}
