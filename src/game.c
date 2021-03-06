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

bool try_moving_hero(Level* level, Entity* hero, Direction direction, GameState* game_state);
bool try_moving_block(Level* level, Entity* block, Direction direction, GameState* game_state);
i32  entities_at_board_position(Entity** occupants, i32 max_allowed, Level* level, Vec2i* pos);

void game_startup(GameState* game_state) {
  BumpAllocator* permanent = &(game_state->bump_permanent);

  u64    level_memory_arena_size = megabytes(MEMORY_ALLOCATION_LEVEL);
  Level* level                   = (Level*)BUMP_ALLOC(permanent, level_memory_arena_size);
  game_state->level              = level;

  // level->bump_allocator.base = level + sizeof(Level);
  // level->bump_allocator.size = level_memory_arena_size - sizeof(Level);
  // level->bump_allocator.used = 0;
  bump_allocator_reset(&(level->bump_allocator), level + sizeof(Level),
                       level_memory_arena_size - sizeof(Level), false, NULL);

  fixed_block_allocator_reset(&level->fixed_block_allocator, &level->bump_allocator, false, NULL);

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
  rona_info("w %d, h %d", w, h);
  level->viewport = rect(-1, -1, w, h);

  level_startup(level, game_state);
  level1_startup(level, game_state);

  RonaGL* gl = game_state->gl;
  renderer_startup(gl, &(game_state->render_struct), permanent);

#ifdef RONA_EDITOR
  BumpAllocator* transient = &(game_state->bump_transient);
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

void stage_from_window_calc(GameState* game_state) {
  RenderStruct* render_struct = &(game_state->render_struct);

  Vec2 stage_from_window_factor; // multiply this
  Vec2 stage_from_window_delta;  // add this

  f32 window_width  = (f32)render_struct->window_width;
  f32 window_height = (f32)render_struct->window_height;
  f32 stage_width   = (f32)render_struct->stage_width;
  f32 stage_height  = (f32)render_struct->stage_height;

  f32 aspect_ratio        = stage_width / stage_height;
  f32 window_aspect_ratio = (f32)render_struct->window_width / (f32)render_struct->window_height;

  if (window_aspect_ratio <= aspect_ratio) {
    // window is narrower than desired

    // world to window
    stage_from_window_factor.x = stage_width / window_width;
    stage_from_window_factor.y = stage_from_window_factor.x;

    f32 y_in_window = stage_height / stage_from_window_factor.x; // height in window pixels of 360
    f32 v           = (f32)window_height - y_in_window;
    f32 v_pad       = v / 2.0f;

    stage_from_window_delta.x = 0.0f;
    stage_from_window_delta.y = -v_pad;
  } else {
    // window is more elongated horizontally than desired
    stage_from_window_factor.x = stage_height / window_height;
    stage_from_window_factor.y = stage_from_window_factor.x;

    f32 x_in_window = stage_width / stage_from_window_factor.y; // width in window pixels of 640
    f32 h           = (f32)window_width - x_in_window;
    f32 h_pad       = h / 2.0f;

    stage_from_window_delta.x = -h_pad;
    stage_from_window_delta.y = 0.0f;
  }

  game_state->stage_from_window_factor = stage_from_window_factor;
  game_state->stage_from_window_delta  = stage_from_window_delta;
}

// changes have been made to the game client and it has now been automatically loaded
void game_lib_load(GameState* game_state) {
  rona_log("permanent base %p, size: %llu, used: %llu", game_state->bump_permanent.base,
           game_state->bump_permanent.size, game_state->bump_permanent.used);
  rona_log("transient base %p, size: %llu, used: %llu", game_state->bump_transient.base,
           game_state->bump_transient.size, game_state->bump_transient.used);

  RonaGL*        gl             = game_state->gl;
  BumpAllocator* bump_transient = &(game_state->bump_transient);
  RenderStruct*  render_struct  = &(game_state->render_struct);

  renderer_lib_load(gl, bump_transient, render_struct);
#ifdef RONA_EDITOR
  editor_lib_load(gl, &editor_state, &(render_struct->shader_editor));
#endif
  stage_from_window_calc(game_state);

  graphic_setup_screen(&(game_state->screen_graphic), gl, (f32)render_struct->stage_width,
                       (f32)render_struct->stage_height);

  level_lib_load(game_state->level, gl, bump_transient, render_struct);

  Colour text_colour_fg;
  colour_from(&text_colour_fg, ColourFormat_RGB, ColourFormat_HSLuv, 10.0f, 40.0f, 60.0f, 1.0f);
  Colour text_colour_bg;
  colour_from(&text_colour_bg, ColourFormat_RGB, ColourFormat_HSLuv, 210.0f, 80.0f, 50.0f, 0.0f);

  TextParams* text_params    = &(game_state->text_params_debug);
  text_params->bump          = bump_transient;
  text_params->render_struct = render_struct;
  text_params->pos           = vec2(0.0f, 140.0f);
  vec4_from_colour(&(text_params->fg), &text_colour_fg);
  vec4_from_colour(&(text_params->bg), &text_colour_bg);
}

// changes have been made to the game client, this old version will be unloaded
void game_lib_unload(GameState* game_state) {
  RenderStruct* render_struct = &(game_state->render_struct);

  level_lib_unload(game_state->level, game_state->gl);
  graphic_teardown(&(game_state->screen_graphic), game_state->gl);

#ifdef RONA_EDITOR
  editor_lib_unload(game_state->gl, &editor_state);
#endif
  renderer_lib_unload(game_state->gl, render_struct);
}

Entity* get_hero(Level* level) {
  RONA_ASSERT(level);

  Entity* e = level->entities;
  while (e->no_further_entities == false) {
    if (e->entity_role == EntityRole_Hero && !e->ignore) {
      return e;
    }
  }

  rona_error("No hero found on level");
  return NULL;
}

void game_step(GameState* game_state) {
  RenderStruct* render_struct = &(game_state->render_struct);
  RonaGL*       gl            = game_state->gl;

  f32 time_delta = (f32)game_state->time_delta / 1000.0f; // in seconds
  f32 fps        = 1.0f / time_delta;

  game_state->bump_transient.used = 0;

  if (key_pressed_ignore_active_flag(game_state->input, Key_F)) {
    game_state->input->active = !game_state->input->active;
  }

  if (game_state->window_resized) {
    stage_from_window_calc(game_state);
  }

  text_render_reset(render_struct);

  Vec2 mouse_pos      = vec2((f32)game_state->input->mouse_pos.x, (f32)game_state->input->mouse_pos.y);
  Vec2 mouse_on_stage = vec2_mul(vec2_add(mouse_pos, game_state->stage_from_window_delta),
                                 game_state->stage_from_window_factor);

  TextParams* text_params = &(game_state->text_params_debug);
  text_params->pos        = vec2(0.0f, 0.0f);
  text_printf(text_params, "%s (%.2f, %.2f) %.0ffps", game_state->mode == GameMode_Edit ? "Edit" : "Play",
              mouse_on_stage.x, mouse_on_stage.y, fps);

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
      nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y, mouse_down(game_state->input, MouseButton_Middle));
      nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, mouse_down(game_state->input, MouseButton_Right));
      // todo: will also need to input all keyboard state here as well
      // see https://immediate-mode-ui.github.io/Nuklear/doc/nuklear.html#nuklear/api/input
    }
    nk_input_end(ctx);

    editor_step(&editor_state, game_state);
  }
#endif

  Level*    level = game_state->level;
  Entity*   hero  = get_hero(level);
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
      moved     = true;
    } else if (key_pressed(game_state->input, Key_Down)) {
      direction = Direction_South;
      moved     = true;
    } else if (key_pressed(game_state->input, Key_Left)) {
      direction = Direction_West;
      moved     = true;
    } else if (key_pressed(game_state->input, Key_Right)) {
      direction = Direction_East;
      moved     = true;
    }
    if (moved) {
      command_transaction_begin(&level->undo_redo);
      try_moving_hero(level, hero, direction, game_state);
      command_transaction_end(&level->undo_redo);
    }
  }

  for (i32 i = 0; i < level->max_num_entities; i++) {
    Entity* e = &(level->entities[i]);

    if (e->no_further_entities) {
      break;
    }
    if (e->ignore) {
      continue;
    }

    // axis aligned distance, not the best but it will do for this simple game
    f32 distance_to_move = e->world_max_speed * time_delta * TILE_WIDTH;

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
      if (e->entity_state == EntityState_MovingThenGone) {
        // this is a block,
        RONA_ASSERT(e->entity_role == EntityRole_Block); // may not always be true
        RONA_ASSERT(e->swallowed_by);
        RONA_ASSERT(e->swallowed_by->entity_role == EntityRole_Pit);

        e->ignore                    = true; // the fallen entity is no longer going to be shown
        e->swallowed_by->entity_role = e->swallower_new_role;
      }
      e->entity_state = EntityState_Standing;
    }
  }

  graphic_entities_regenerate_geometry(level, gl, render_struct);

  text_send_to_gpu(render_struct, gl);

  renderer_render(game_state);
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

bool try_moving_hero(Level* level, Entity* hero, Direction direction, GameState* game_state) {
  Vec2i new_pos = vec2i_add_direction(&hero->board_pos, direction);

  Tile* tile = tile_from_world_tile_space(level, new_pos);
  if (tile->type == TileType_Void) {
    return false;
  }

  // check if this will push any other entities
  Entity* occupants[MAX_OCCUPANTS_ALLOWED];
  i32     num_occupants = entities_at_board_position(occupants, MAX_OCCUPANTS_ALLOWED, level, &new_pos);
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
      if (!block_can_move) {
        return false;
      }
    }
  }

  {
    Command* command = command_add(&level->undo_redo, &level->fixed_block_allocator, game_state);

    command->type   = CommandType_EntityMove;
    command->entity = hero;

    EntityMoveParams* old_params = &command->params.entity_move.old_params;
    EntityMoveParams* new_params = &command->params.entity_move.new_params;

    old_params->entity_facing = hero->entity_facing;
    if (direction == Direction_West) {
      new_params->entity_facing = EntityFacing_Left;
    } else if (direction == Direction_East) {
      new_params->entity_facing = EntityFacing_Right;
    } else {
      new_params->entity_facing = hero->entity_facing;
    }

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

bool try_moving_block(Level* level, Entity* block, Direction direction, GameState* game_state) {
  Vec2i new_pos = vec2i_add_direction(&block->board_pos, direction);

  Tile* tile = tile_from_world_tile_space(level, new_pos);
  if (tile->type == TileType_Void) {
    return false;
  }

  // check if this will push any other entities
  bool    is_occupier_block = false;
  bool    is_occupier_pit   = false;
  Entity* occupants[MAX_OCCUPANTS_ALLOWED];
  i32     num_occupants = entities_at_board_position(occupants, MAX_OCCUPANTS_ALLOWED, level, &new_pos);
  if (num_occupants > 0) {

    Entity* pit = NULL;

    for (i32 i = 0; i < num_occupants; i++) {
      if (occupants[i]->entity_role == EntityRole_Block) {
        is_occupier_block = true;
      }
      if (occupants[i]->entity_role == EntityRole_Pit) {
        pit             = occupants[i];
        is_occupier_pit = true;
      }
      // if (occupants[i]->entity_role == EntityRole_FilledPit) {
      //   just treat this as a normal floor
      // }
    }

    if (is_occupier_pit) {
      // move the block over the pit, then transition to a filled pit
      //
      Command* command = command_add(&level->undo_redo, &level->fixed_block_allocator, game_state);

      command->type   = CommandType_EntityMoveThenSwallow;
      command->entity = block;

      EntityMoveParams* old_params = &command->params.entity_move_then_swallow.old_params;
      EntityMoveParams* new_params = &command->params.entity_move_then_swallow.new_params;

      command->params.entity_move_then_swallow.swallower          = pit;
      command->params.entity_move_then_swallow.swallower_old_role = pit->entity_role;
      command->params.entity_move_then_swallow.swallower_new_role = EntityRole_FilledPit;

      old_params->board_pos = block->board_pos;
      new_params->board_pos = new_pos;

      old_params->world_target = block->world_target;
      world_from_board(&new_params->world_target, new_pos.x, new_pos.y, block->world_target.z);

      old_params->world_pos = block->world_pos;
      new_params->world_pos = new_params->world_target;

      old_params->entity_state = block->entity_state;
      new_params->entity_state = EntityState_MovingThenGone;

      command_execute(command, CommandExecute_Play, game_state);

      return true;
    } else if (is_occupier_block) {
      // can't move the block, there's another block in the way
      return false;
    }
  }

  // happy path - move onto updating the block position
  Command* command = command_add(&level->undo_redo, &level->fixed_block_allocator, game_state);

  command->type   = CommandType_EntityMove;
  command->entity = block;

  EntityMoveParams* old_params = &command->params.entity_move.old_params;
  EntityMoveParams* new_params = &command->params.entity_move.new_params;

  old_params->board_pos = block->board_pos;
  new_params->board_pos = new_pos;

  old_params->world_target = block->world_target;
  world_from_board(&new_params->world_target, new_pos.x, new_pos.y, block->world_target.z);

  old_params->world_pos = block->world_pos;
  new_params->world_pos = new_params->world_target;

  old_params->entity_state = block->entity_state;
  new_params->entity_state = EntityState_Moving;

  command_execute(command, CommandExecute_Play, game_state);
  return true;
}

// how many entities are there on the level at the given position?
// returns the amount and fills in the entities array
i32 entities_at_board_position(Entity** occupants, i32 max_allowed, Level* level, Vec2i* pos) {
  i32 num_occupants = 0;
  for (i32 i = 0; i < level->max_num_entities; i++) {
    if (level->entities[i].no_further_entities == true) {
      return num_occupants;
    }

    if (level->entities[i].ignore) {
      continue;
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
