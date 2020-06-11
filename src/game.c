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

void game_startup(GameState* game_state) {
  game_state->mesh_hero = (Mesh *)ARENA_ALLOC(&game_state->storage_permanent, sizeof(Mesh));
  game_state->mesh_block = (Mesh *)ARENA_ALLOC(&game_state->storage_permanent, sizeof(Mesh));

  u64 level_memory_arena_size = megabytes(64);
  game_state->level = (Level *)ARENA_ALLOC(&game_state->storage_permanent, level_memory_arena_size);
  game_state->level->mem.base = game_state->level + sizeof(Level);
  game_state->level->mem.size = level_memory_arena_size - sizeof(Level);
  game_state->level->mem.used = 0;

  level1_startup(game_state->level, game_state);

  renderer_startup(game_state->gl);
}

void game_shutdown(GameState* game_state) {
  level1_shutdown(game_state->level);
  renderer_shutdown(game_state->gl);
}

// changes have been made to the game client and it has now been automatically loaded
void game_lib_load(GameState* game_state) {
  renderer_lib_load(game_state->gl);
  mesh_block_lib_load(game_state->mesh_block, game_state->gl, &game_state->storage_transient);
  mesh_hero_lib_load(game_state->mesh_hero, game_state->gl, &game_state->storage_transient);
  level1_lib_load(game_state->level, game_state->gl, &game_state->storage_transient);
}

// changes have been made to the game client, this old version will be unloaded
void game_lib_unload(GameState* game_state) {
  level1_lib_unload(game_state->level, game_state->gl);
  mesh_hero_lib_unload(game_state->mesh_hero, game_state->gl);
  mesh_block_lib_unload(game_state->mesh_block, game_state->gl);
  renderer_lib_unload(game_state->gl);
}

Entity* get_hero(Level *level) {
  RONA_ASSERT(level);

  Entity *e = level->entities;
  while(e->exists) {
    if(e->entity_type == EntityType_Hero) {
      return e;
    }
  }

  RONA_ERROR("No hero found on level\n");
  return NULL;
}

void game_step(GameState* game_state) {
  game_state->storage_transient.used = 0;

  if (key_down(game_state->input, Key_Escape) ||
      key_down(game_state->input, Key_Q)) {
    game_state->quit_game = true;
    return;
  }
  if (key_pressed(game_state->input, Key_T)) {
    RONA_OUT("press");
    fflush(stdout);
  }

  Level *level = game_state->level;
  Entity *e = get_hero(level);
  bool moved = false;

  if (key_pressed(game_state->input, Key_Up) &&
      is_valid_move_direction(level, e, e->board_pos.x, e->board_pos.y + 1)) {
    e->board_pos.y += 1;
    moved = true;
  }
  if (key_pressed(game_state->input, Key_Down) &&
      is_valid_move_direction(level, e, e->board_pos.x, e->board_pos.y - 1)) {
    e->board_pos.y -= 1;
    moved = true;
  }
  if (key_pressed(game_state->input, Key_Left) &&
      is_valid_move_direction(level, e, e->board_pos.x - 1, e->board_pos.y)) {
    e->board_pos.x -= 1;
    moved = true;
  }
  if (key_pressed(game_state->input, Key_Right) &&
      is_valid_move_direction(level, e, e->board_pos.x + 1, e->board_pos.y)) {
    e->board_pos.x += 1;
    moved = true;
  }


  // board to world transform
  if (moved) {
    e->world_target.x = (f32)e->board_pos.x;
    e->world_target.y = (f32)e->board_pos.y;
    e->entity_state = EntityState_Moving;
    // RONA_INFO("Moving");
  }

  f32 time_delta = (f32)game_state->time_delta / 1000.0f;
  // axis aligned distance, not the best but it will do for this simple game
  f32 distance_to_move = e->world_max_speed * time_delta;

  if (e->entity_state == EntityState_Moving) {
    if (e->world_pos.x < e->world_target.x) {
      e->world_pos.x += distance_to_move;
      if (e->world_pos.x >= e->world_target.x) {
        e->world_pos.x = e->world_target.x;
      }
    }
    if (e->world_pos.y < e->world_target.y) {
      e->world_pos.y += distance_to_move;
      if (e->world_pos.y >= e->world_target.y) {
        e->world_pos.y = e->world_target.y;
      }
    }
    if (e->world_pos.x > e->world_target.x) {
      e->world_pos.x -= distance_to_move;
      if (e->world_pos.x <= e->world_target.x) {
        e->world_pos.x = e->world_target.x;
      }
    }
    if (e->world_pos.y > e->world_target.y) {
      e->world_pos.y -= distance_to_move;
      if (e->world_pos.y <= e->world_target.y) {
        e->world_pos.y = e->world_target.y;
      }
    }

    if (e->world_pos.x == e->world_target.x && e->world_pos.y == e->world_target.y) {
      e->entity_state = EntityState_Standing;
    }
  }

  renderer_render(game_state->gl, game_state->level, game_state->window_width, game_state->window_height);
}
