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

void game_startup(GameState* game_state) {
  u64    level_memory_arena_size = megabytes(MEMORY_ALLOCATION_LEVEL);
  Level* level = (Level*)BUMP_ALLOC(&game_state->arena_permanent, level_memory_arena_size);
  game_state->level = level;

  level->bump_allocator.base = level + sizeof(Level);
  level->bump_allocator.size = level_memory_arena_size - sizeof(Level);
  level->bump_allocator.used = 0;
  fixed_block_allocator_reset(&level->fixed_block_allocator, &level->bump_allocator);

  // work out how many tiles are required to cover the stage along each dimension
  //
  i32 w = STAGE_WIDTH / TILE_WIDTH;
  if (w * TILE_WIDTH < STAGE_WIDTH) {
    w++;
  }
  i32 h = STAGE_HEIGHT / TILE_HEIGHT;
  if (h * TILE_HEIGHT < STAGE_HEIGHT) {
    h++;
  }
  RONA_INFO("w %d, h %d\n", w, h);
  level->viewport = rect(-3, -3, w, h);

  level_startup(level, game_state);
  level1_startup(level, game_state);

  RonaGL* gl = game_state->gl;
  renderer_startup(gl, &(game_state->render_struct), &(game_state->arena_permanent));

#ifdef RONA_EDITOR
  BumpAllocator* permanent = &(game_state->arena_permanent);
  BumpAllocator* transient = &(game_state->arena_transient);
  editor_startup(gl, &editor_state, permanent, transient);
  editor_changed_level(&editor_state, level);
#endif /*  RONA_EDITOR  */
}

void game_shutdown(GameState* game_state) {
#ifdef RONA_EDITOR
  editor_shutdown(game_state->gl, &editor_state);
#endif

  level1_shutdown(game_state->level);
  level_shutdown(game_state->level);
  renderer_shutdown(game_state->gl);
}

Vec2 stage_from_window(GameState* game_state, f32 x, f32 y) {
  return vec2_mul(vec2_add(vec2(x, y), game_state->stage_from_window_delta),
                  game_state->stage_from_window_factor);
}

void stage_from_window_calc(GameState* game_state) {
  RenderStruct* render_struct = &(game_state->render_struct);

  Vec2 stage_from_window_factor; // multiply this
  Vec2 stage_from_window_delta;  // add this

  f32 window_width = (f32)render_struct->window_width;
  f32 window_height = (f32)render_struct->window_height;
  f32 stage_width = (f32)render_struct->stage_width;
  f32 stage_height = (f32)render_struct->stage_height;

  f32 aspect_ratio = stage_width / stage_height;
  f32 window_aspect_ratio = (f32)render_struct->window_width / (f32)render_struct->window_height;

  if (window_aspect_ratio <= aspect_ratio) {
    // window is narrower than desired

    // world to window
    stage_from_window_factor.x = stage_width / window_width;
    stage_from_window_factor.y = stage_from_window_factor.x;

    f32 y_in_window = stage_height / stage_from_window_factor.x; // height in window pixels of 360
    f32 v = (f32)window_height - y_in_window;
    f32 v_pad = v / 2.0f;

    stage_from_window_delta.x = 0.0f;
    stage_from_window_delta.y = -v_pad;
  } else {
    // window is more elongated horizontally than desired
    stage_from_window_factor.x = stage_height / window_height;
    stage_from_window_factor.y = stage_from_window_factor.x;

    f32 x_in_window = stage_width / stage_from_window_factor.y; // width in window pixels of 640
    f32 h = (f32)window_width - x_in_window;
    f32 h_pad = h / 2.0f;

    stage_from_window_delta.x = -h_pad;
    stage_from_window_delta.y = 0.0f;
  }

  game_state->stage_from_window_factor = stage_from_window_factor;
  game_state->stage_from_window_delta = stage_from_window_delta;
}

// changes have been made to the game client and it has now been automatically loaded
void game_lib_load(GameState* game_state) {
  // RONA_LOG("base %p, size: %llu, used: %llu\n", game_state->arena_permanent.base,
  //          game_state->arena_permanent.size, game_state->arena_permanent.used);

  RonaGL*        gl = game_state->gl;
  BumpAllocator* bump_transient = &(game_state->arena_transient);
  Tileset*       tileset = &(game_state->render_struct.tileset);
  RenderStruct*  render_struct = &(game_state->render_struct);

  renderer_lib_load(gl, bump_transient, render_struct);
#ifdef RONA_EDITOR
  editor_lib_load(gl, &editor_state, &(render_struct->shader_editor));
#endif
  stage_from_window_calc(game_state);

  Colour transparent = colour_make(ColourFormat_RGB, 0.0f, 0.0f, 0.0f, 0.0f);
  Colour red = colour_make(ColourFormat_HSLuv, 400.0f, 90.0f, 30.0f, 1.0f);

  graphic_lib_load_single_tile(&(game_state->graphic_pit), gl, tileset, TS_PressurePadActivated,
                               red, transparent);
  graphic_lib_load_single_tile(&(game_state->graphic_block), gl, tileset, TS_Block, red,
                               transparent);
  graphic_lib_load_single_tile(&(game_state->graphic_hero), gl, tileset, TS_Hero, red, transparent);
  graphic_screen_lib_load(&(game_state->graphic_screen), gl, render_struct);

  level1_lib_load(game_state->level, gl, bump_transient, tileset);

  Colour text_colour_fg;
  colour_from(&text_colour_fg, ColourFormat_RGB, ColourFormat_HSLuv, 50.0f, 80.0f, 60.0f, 1.0f);
  Colour text_colour_bg;
  colour_from(&text_colour_bg, ColourFormat_RGB, ColourFormat_HSLuv, 210.0f, 80.0f, 50.0f, 0.0f);

  TextParams* text_params = &(game_state->text_params_debug);
  text_params->bump = bump_transient;
  text_params->render_struct = render_struct;
  text_params->pos = vec2(0.0f, 140.0f);
  vec4_from_colour(&(text_params->fg), &text_colour_fg);
  vec4_from_colour(&(text_params->bg), &text_colour_bg);
}

// changes have been made to the game client, this old version will be unloaded
void game_lib_unload(GameState* game_state) {
  RenderStruct* render_struct = &(game_state->render_struct);

  level1_lib_unload(game_state->level, game_state->gl);
  graphic_screen_lib_unload(&(game_state->graphic_screen), game_state->gl);
  graphic_lib_unload(&(game_state->graphic_hero), game_state->gl);
  graphic_lib_unload(&(game_state->graphic_block), game_state->gl);
  graphic_lib_unload(&(game_state->graphic_pit), game_state->gl);

#ifdef RONA_EDITOR
  editor_lib_unload(game_state->gl, &editor_state);
#endif
  renderer_lib_unload(game_state->gl, render_struct);
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
  RenderStruct* render_struct = &(game_state->render_struct);
  RonaGL*       gl = game_state->gl;

  game_state->arena_transient.used = 0;

  if (key_pressed_ignore_active_flag(game_state->input, Key_F)) {
    game_state->input->active = !game_state->input->active;
  }

  if (game_state->window_resized) {
    stage_from_window_calc(game_state);
  }

  text_render_reset(render_struct);

  Vec2        mouse_on_stage = stage_from_window(game_state, (f32)game_state->input->mouse_pos.x,
                                          (f32)game_state->input->mouse_pos.y);
  TextParams* text_params = &(game_state->text_params_debug);
  text_params->pos = vec2(0.0f, 0.0f);
  text_printf(text_params, "%s (%.2f, %.2f)", game_state->mode == GameMode_Edit ? "Edit" : "Play",
              mouse_on_stage.x, mouse_on_stage.y);

  if (!game_state->input->active) {
    text_params->pos = vec2(0.0f, text_params->pos.y + TILE_HEIGHT);
    text_printf(text_params, "Input Ignored\n");
  }

  if (key_down(game_state->input, Key_Escape) || key_down(game_state->input, Key_Q)) {
    game_state->quit_game = true;
    return;
  }
  if (key_pressed(game_state->input, Key_M)) {
    game_state->mode = game_state->mode == GameMode_Edit ? GameMode_Play : GameMode_Edit;
  }

#ifdef RONA_EDITOR
  if (game_state->mode == GameMode_Edit) {
    struct nk_context* ctx = &editor_state.ctx;
    nk_input_begin(ctx);
    {
      int x = (int)game_state->input->mouse_pos.x;
      int y = (int)game_state->input->mouse_pos.y;
      nk_input_motion(ctx, x, y);
      nk_input_button(ctx, NK_BUTTON_LEFT, x, y, mouse_down(game_state->input, MouseButton_Left));
      nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y,
                      mouse_down(game_state->input, MouseButton_Middle));
      nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, mouse_down(game_state->input, MouseButton_Right));
      // todo: will also need to input all keyboard state here as well
      // see https://immediate-mode-ui.github.io/Nuklear/doc/nuklear.html#nuklear/api/input
    }
    nk_input_end(ctx);

    editor_step(&editor_state, game_state);
  }
#endif

  Level*    level = game_state->level;
  Entity*   hero = get_hero(level);
  Direction direction;
  bool      moved = false;

  /*
   * NOTE: The window event: LostFocus isn't always quick or reliable,
   * so I could end up moving focus to Emacs, and having this program
   * pick up those key presses as well
   */

  if (key_pressed(game_state->input, Key_Z)) {
    command_undo(&level->undo_redo, game_state);
  }

  if (key_pressed(game_state->input, Key_A)) {
    command_redo(&level->undo_redo, game_state);
  }

#if 0
  if (key_pressed(game_state->input, Key_D)) {
    command_debug(&level->undo_redo);
  }
#endif

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
      command_transaction_begin(&level->undo_redo);
      try_moving_hero(level, hero, direction, game_state);
      command_transaction_end(&level->undo_redo);
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

    // if (e->entity_state == EntityState_Moving) {
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

  text_send_to_gpu(render_struct, gl);

  renderer_render(game_state);
}
