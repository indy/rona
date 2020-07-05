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
  game_state->mesh_screen = (Mesh*)ARENA_ALLOC(&game_state->storage_permanent, sizeof(Mesh));
  game_state->mesh_hero = (Mesh*)ARENA_ALLOC(&game_state->storage_permanent, sizeof(Mesh));
  game_state->mesh_block = (Mesh*)ARENA_ALLOC(&game_state->storage_permanent, sizeof(Mesh));
  game_state->mesh_pit = (Mesh*)ARENA_ALLOC(&game_state->storage_permanent, sizeof(Mesh));

  u64 level_memory_arena_size = megabytes(64);
  game_state->level = (Level*)ARENA_ALLOC(&game_state->storage_permanent, level_memory_arena_size);
  game_state->level->mem.base = game_state->level + sizeof(Level);
  game_state->level->mem.size = level_memory_arena_size - sizeof(Level);
  game_state->level->mem.used = 0;

  level1_startup(game_state->level, game_state);

  renderer_startup(game_state->gl, &(game_state->render_struct), &(game_state->storage_permanent));
}

void game_shutdown(GameState* game_state) {
  level1_shutdown(game_state->level);
  renderer_shutdown(game_state->gl);
}

Vec2 render_texture_from_window(GameState *game_state, f32 x, f32 y) {
  Vec2 pos;
  pos.x = x;
  pos.y = (f32)game_state->render_struct.window_height - y;

  return vec2_mul(vec2_add(pos, game_state->render_texture_from_window_delta),
                  game_state->render_texture_from_window_factor);
}

void render_texture_from_window_calc(GameState* game_state) {
  RenderStruct *render_struct = &(game_state->render_struct);

  Vec2 render_texture_from_window_factor;   // multiply this
  Vec2 render_texture_from_window_delta;    // add this

  f32 window_width = (f32)render_struct->window_width;
  f32 window_height = (f32)render_struct->window_height;
  f32 render_texture_width = (f32)render_struct->render_texture_width;
  f32 render_texture_height = (f32)render_struct->render_texture_height;

  f32 aspect_ratio = render_texture_width / render_texture_height;
  f32 window_aspect_ratio = (f32)render_struct->window_width / (f32)render_struct->window_height;

  if (window_aspect_ratio <= aspect_ratio) {
    // window is narrower than desired

    // world to window
    render_texture_from_window_factor.x = render_texture_width / window_width;
    render_texture_from_window_factor.y = render_texture_from_window_factor.x;

    f32 y_in_window = render_texture_height / render_texture_from_window_factor.x; // height in window pixels of 360
    f32 v = (f32)window_height - y_in_window;
    f32 v_pad = v / 2.0f;

    render_texture_from_window_delta.x = 0.0f;
    render_texture_from_window_delta.y = -v_pad;
  } else {
    // window is more elongated horizontally than desired
    render_texture_from_window_factor.x = render_texture_height / window_height;
    render_texture_from_window_factor.y = render_texture_from_window_factor.x;

    f32 x_in_window = render_texture_width / render_texture_from_window_factor.y; // width in window pixels of 640
    f32 h = (f32)window_width - x_in_window;
    f32 h_pad = h / 2.0f;

    render_texture_from_window_delta.x = -h_pad;
    render_texture_from_window_delta.y = 0.0f;
  }

  game_state->render_texture_from_window_factor = render_texture_from_window_factor;
  game_state->render_texture_from_window_delta = render_texture_from_window_delta;
}

// changes have been made to the game client and it has now been automatically loaded
void game_lib_load(GameState* game_state) {
  RonaGl*       gl = game_state->gl;
  MemoryArena*  arena = &(game_state->storage_transient);
  Tileset*      tileset = &(game_state->render_struct.tileset);
  RenderStruct* render_struct = &(game_state->render_struct);

  renderer_lib_load(gl, arena, render_struct);

  render_texture_from_window_calc(game_state);

  Colour transparent = colour_make(ColourFormat_RGB, 0.0f, 0.0f, 0.0f, 0.0f);
  Colour red = colour_make(ColourFormat_HSLuv, 400.0f, 90.0f, 30.0f, 1.0f);

  mesh_lib_load_single_tile(game_state->mesh_pit, gl, tileset, TS_PressurePadActivated, red,
                            transparent);
  mesh_lib_load_single_tile(game_state->mesh_block, gl, tileset, TS_Block, red, transparent);
  mesh_lib_load_single_tile(game_state->mesh_hero, gl, tileset, TS_Hero, red, transparent);
  mesh_screen_lib_load(game_state->mesh_screen, gl, render_struct);
  level1_lib_load(game_state->level, gl, arena, tileset);
}

// changes have been made to the game client, this old version will be unloaded
void game_lib_unload(GameState* game_state) {
  level1_lib_unload(game_state->level, game_state->gl);
  mesh_screen_lib_unload(game_state->mesh_screen, game_state->gl);
  mesh_lib_unload(game_state->mesh_hero, game_state->gl);
  mesh_lib_unload(game_state->mesh_block, game_state->gl);
  mesh_lib_unload(game_state->mesh_pit, game_state->gl);
  renderer_lib_unload(game_state->gl);
}

Entity* get_hero(Level* level) {
  RONA_ASSERT(level);

  Entity* e = level->entities;
  while (e->exists) {
    if (e->entity_type == EntityType_Hero) {
      return e;
    }
  }

  RONA_ERROR("No hero found on level\n");
  return NULL;
}

void game_step(GameState* game_state) {
  game_state->storage_transient.used = 0;

  if (game_state->window_resized) {
    render_texture_from_window_calc(game_state);
  }

  RenderStruct *render_struct = &(game_state->render_struct);
  RonaGl *gl = game_state->gl;

  text_reset(render_struct);

  Colour ground_colour_fg;
  colour_from(&ground_colour_fg, ColourFormat_RGB, ColourFormat_HSLuv, 50.0f, 80.0f, 60.0f, 1.0f);
  Colour ground_colour_bg;
  colour_from(&ground_colour_bg, ColourFormat_RGB, ColourFormat_HSLuv, 210.0f, 80.0f, 50.0f, 0.0f);

  TextParams text_params;
  text_params.arena = &(game_state->storage_transient);
  text_params.render_struct = render_struct;
  text_params.pos = vec2(0.0f, 140.0f);
  vec4_from_colour(&(text_params.fg), &ground_colour_fg);
  vec4_from_colour(&(text_params.bg), &ground_colour_bg);


  Vec2 mouse_rt = render_texture_from_window(game_state,
                                           (f32)game_state->input->mouse_pos.x,
                                           (f32)game_state->input->mouse_pos.y);
  // text_params.pos = vec2(200.0f, 248.0f);
  // text_printf(&text_params, "mouse in world (%.2f, %.2f)", mouse_rt.x, mouse_rt.y);

  text_params.pos = vec2(16.0f, 160.0f);
  text_printf(&text_params, "here are some words XYZ");

  if (key_down(game_state->input, Key_Escape) || key_down(game_state->input, Key_Q)) {
    game_state->quit_game = true;
    return;
  }
  if (key_pressed(game_state->input, Key_T)) {
    RONA_OUT("press");
    fflush(stdout);
  }

  Level*    level = game_state->level;
  Entity*   hero = get_hero(level);
  Direction direction;
  bool      moved = false;

  if (hero->entity_state == EntityState_Standing) {
    if (key_pressed(game_state->input, Key_Up)) {
      direction = Direction_North;
      moved = true;
    } else if (key_pressed(game_state->input, Key_Down)) {
      direction = Direction_South;
      moved = true;
    } else if (key_pressed(game_state->input, Key_Left)) {
      direction = Direction_West;
      moved = true;
    } else if (key_pressed(game_state->input, Key_Right)) {
      direction = Direction_East;
      moved = true;
    }
    if (moved) {
      try_moving_hero(level, hero, direction);
    }
  }

  f32 time_delta = (f32)game_state->time_delta / 1000.0f;

  for (i32 i = 0; i < level->max_num_entities; i++) {
    Entity* e = &(level->entities[i]);
    if (e->exists == false) {
      break;
    }

    // axis aligned distance, not the best but it will do for this simple game
    f32 distance_to_move = e->world_max_speed * time_delta * TILE_WIDTH;

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
  }

  text_send_to_gpu(render_struct, gl);

  renderer_render(game_state->gl, game_state->level, &game_state->render_struct,
                  game_state->mesh_screen);
}
