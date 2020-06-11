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

bool is_valid_move_direction(Level *level, Entity *entity, i32 x, i32 y) {

  if (x < 0 || x >= level->width) {
    return false;
  }
  if (y < 0 || y >= level->height) {
    return false;
  }

  if (level->tiles[x + (y * level->width)] == TileType_Void) {
    return false;
  }

  return true;
}


// place an entity at the given board positions
void entity_place(Entity *entity, i32 board_pos_x, i32 board_pos_y) {
  entity->board_pos.x = board_pos_x;
  entity->board_pos.y = board_pos_y;
  entity->world_pos.x = (f32)board_pos_x;
  entity->world_pos.y = (f32)board_pos_y;
  entity->world_target.x = (f32)board_pos_x;
  entity->world_target.y = (f32)board_pos_y;
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
  level->tiles = (TileType *)ARENA_ALLOC(&(level->mem), sizeof(TileType) * num_tiles);

  for (i32 j = 0; j < height; j++) {

    char *plan_line = layout[height - 1 - j];

    for (i32 i = 0; i < dbl_width; i+=2) {
      i32 tile_index = (i / 2) + (j * width);

      if (plan_line[i] != ' ') {
        i32 tile_x = i / 2;
        i32 tile_y = j;

        level->tiles[tile_index] = TileType_Floor;

        if (plan_line[i] == 'H') { // hero
          Entity *hero = &(level->entities[0]);

          have_hero = true;
          hero->exists = true;
          hero->entity_type = EntityType_Hero;
          hero->entity_state = EntityState_Standing;
          hero->mesh = game_state->mesh_hero;

          entity_place(hero, tile_x, tile_y);

          hero->world_max_speed = 18.0f;

          Colour hero_colour;
          colour_from(&hero_colour, ColourFormat_sRGB, ColourFormat_HSLuv, 10.0f, 90.0f, 50.0f, 1.0f);
          hero->colour.r = hero_colour.element[0];
          hero->colour.g = hero_colour.element[1];
          hero->colour.b = hero_colour.element[2];
          hero->colour.a = hero_colour.element[3];

        } else if (plan_line[i] == 'B') { // block
          RONA_ASSERT(next_non_hero_entity_index < level->max_num_entities);

          Entity *block = &(level->entities[next_non_hero_entity_index++]);

          block->exists = true;
          block->entity_type = EntityType_Block;
          block->entity_state = EntityState_Standing;
          block->mesh = game_state->mesh_block;

          entity_place(block, tile_x, tile_y);

          block->world_max_speed = 18.0f;


          Colour block_colour;
          colour_from(&block_colour, ColourFormat_sRGB, ColourFormat_HSLuv, 10.0f, 80.0f, 20.0f, 1.0f);
          block->colour.r = block_colour.element[0];
          block->colour.g = block_colour.element[1];
          block->colour.b = block_colour.element[2];
          block->colour.a = block_colour.element[3];

        } else if (plan_line[i] == 'U') { // pit
          RONA_ASSERT(next_non_hero_entity_index < level->max_num_entities);

          Entity *pit = &(level->entities[next_non_hero_entity_index++]);

          pit->exists = true;
          pit->entity_type = EntityType_Pit;
          pit->entity_state = EntityState_Standing;
          pit->mesh = game_state->mesh_pit;

          entity_place(pit, tile_x, tile_y);

          pit->world_max_speed = 18.0f;

          Colour pit_colour;
          colour_from(&pit_colour, ColourFormat_sRGB, ColourFormat_HSLuv, 10.0f, 80.0f, 2.0f, 1.0f);
          pit->colour.r = pit_colour.element[0];
          pit->colour.g = pit_colour.element[1];
          pit->colour.b = pit_colour.element[2];
          pit->colour.a = pit_colour.element[3];
        }
      } else {
        level->tiles[tile_index] = TileType_Void;
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
    if (level->tiles[i] == TileType_Floor) {
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
      if (level->tiles[i + (j*level->width)] == TileType_Floor) {
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
