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
  game_state->mesh_screen = (Mesh*)BUMP_ALLOC(&game_state->arena_permanent, sizeof(Mesh));
  game_state->mesh_hero = (Mesh*)BUMP_ALLOC(&game_state->arena_permanent, sizeof(Mesh));
  game_state->mesh_block = (Mesh*)BUMP_ALLOC(&game_state->arena_permanent, sizeof(Mesh));
  game_state->mesh_pit = (Mesh*)BUMP_ALLOC(&game_state->arena_permanent, sizeof(Mesh));

  u64 level_memory_arena_size = megabytes(MEMORY_ALLOCATION_LEVEL);
  game_state->level = (Level*)BUMP_ALLOC(&game_state->arena_permanent, level_memory_arena_size);
  game_state->level->mem.base = game_state->level + sizeof(Level);
  game_state->level->mem.size = level_memory_arena_size - sizeof(Level);
  game_state->level->mem.used = 0;

  level1_startup(game_state->level, game_state);

  renderer_startup(game_state->gl, &(game_state->render_struct), &(game_state->arena_permanent));

#ifdef RONA_NUKLEAR
  // allocate some memory for nuklear
  usize nuklear_memory_size = megabytes(MEMORY_ALLOCATION_NUKLEAR);
  void* nuklear_memory = (void*)BUMP_ALLOC(&game_state->arena_permanent, nuklear_memory_size);

  // align memory with nk_draw_command
  const nk_size cmd_align = NK_ALIGNOF(struct nk_draw_command);
  u64           align_mask = (cmd_align << 1) - 1;
  void* aligned_nuklear_memory = nuklear_memory - ((u64)nuklear_memory & align_mask) + cmd_align;
  nuklear_memory_size -= aligned_nuklear_memory - nuklear_memory;

  RonaGL* gl = game_state->gl;
  device_init(gl, &nuklear_state);

  struct nk_font*       font_to_use;
  const void*           image;
  int                   w, h;
  struct nk_font_config cfg = nk_font_config(0);
  cfg.oversample_h = 3;
  cfg.oversample_v = 2;
  /* Loading one font with different heights is only required if you want higher
   * quality text otherwise you can just set the font height directly
   * e.g.: device.ctx->style.font.height = 20. */

  struct nk_font_atlas* atlas = &(nuklear_state.atlas);

  nk_font_atlas_init_default(atlas);
  nk_font_atlas_begin(atlas);

#ifdef RONA_NUKLEAR_DEMO_WITH_IMAGES
  const char* font_ttf = "assets/fonts/Roboto-Regular.ttf";
  nuklear_media.font_14 = nk_font_atlas_add_from_file(atlas, font_ttf, 14.0f, &cfg);
  nuklear_media.font_18 = nk_font_atlas_add_from_file(atlas, font_ttf, 18.0f, &cfg);
  nuklear_media.font_20 = nk_font_atlas_add_from_file(atlas, font_ttf, 20.0f, &cfg);
  nuklear_media.font_22 = nk_font_atlas_add_from_file(atlas, font_ttf, 22.0f, &cfg);
  font_to_use = nuklear_media.font_14;
#else
  nuklear_media.default_font = nk_font_atlas_add_default(atlas, 14.0f, &cfg);
  font_to_use = nuklear_media.default_font;
#endif /*  #ifdef RONA_NUKLEAR_DEMO_WITH_IMAGES */

  image = nk_font_atlas_bake(atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
  device_upload_atlas(gl, &nuklear_state, image, w, h);
  nk_font_atlas_end(atlas, nk_handle_id((int)nuklear_state.font_tex), &nuklear_state.null);

  nk_init_fixed(&nuklear_state.ctx, aligned_nuklear_memory, (nk_size)nuklear_memory_size,
                &font_to_use->handle);

#ifdef RONA_NUKLEAR_DEMO_WITH_IMAGES
  /* icons */
  gl->enable(GL_TEXTURE_2D);
  nuklear_media.unchecked = icon_load(gl, "assets/icon/unchecked.png");
  nuklear_media.checked = icon_load(gl, "assets/icon/checked.png");
  nuklear_media.rocket = icon_load(gl, "assets/icon/rocket.png");
  nuklear_media.cloud = icon_load(gl, "assets/icon/cloud.png");
  nuklear_media.pen = icon_load(gl, "assets/icon/pen.png");
  nuklear_media.play = icon_load(gl, "assets/icon/play.png");
  nuklear_media.pause = icon_load(gl, "assets/icon/pause.png");
  nuklear_media.stop = icon_load(gl, "assets/icon/stop.png");
  nuklear_media.next = icon_load(gl, "assets/icon/next.png");
  nuklear_media.prev = icon_load(gl, "assets/icon/prev.png");
  nuklear_media.tools = icon_load(gl, "assets/icon/tools.png");
  nuklear_media.dir = icon_load(gl, "assets/icon/directory.png");
  nuklear_media.copy = icon_load(gl, "assets/icon/copy.png");
  nuklear_media.convert = icon_load(gl, "assets/icon/export.png");
  nuklear_media.del = icon_load(gl, "assets/icon/delete.png");
  nuklear_media.edit = icon_load(gl, "assets/icon/edit.png");
  nuklear_media.menu[0] = icon_load(gl, "assets/icon/home.png");
  nuklear_media.menu[1] = icon_load(gl, "assets/icon/phone.png");
  nuklear_media.menu[2] = icon_load(gl, "assets/icon/plane.png");
  nuklear_media.menu[3] = icon_load(gl, "assets/icon/wifi.png");
  nuklear_media.menu[4] = icon_load(gl, "assets/icon/settings.png");
  nuklear_media.menu[5] = icon_load(gl, "assets/icon/volume.png");

  {
    int i;
    for (i = 0; i < 9; ++i) {
      char buffer[256];
      sprintf(buffer, "assets/images/image%d.png", (i + 1));
      nuklear_media.images[i] = icon_load(gl, buffer);
    }
  }
#endif /*  RONA_NUKLEAR_DEMO_WITH_IMAGES   */
#endif /*  RONA_NUKLEAR  */
}

void game_shutdown(GameState* game_state) {
#ifdef RONA_NUKLEAR

#ifdef RONA_NUKLEAR_DEMO_WITH_IMAGES
  RonaGL* gl = game_state->gl;
  gl->deleteTextures(1, (const GLuint*)&nuklear_media.unchecked.handle.id);
  gl->deleteTextures(1, (const GLuint*)&nuklear_media.checked.handle.id);
  gl->deleteTextures(1, (const GLuint*)&nuklear_media.rocket.handle.id);
  gl->deleteTextures(1, (const GLuint*)&nuklear_media.cloud.handle.id);
  gl->deleteTextures(1, (const GLuint*)&nuklear_media.pen.handle.id);
  gl->deleteTextures(1, (const GLuint*)&nuklear_media.play.handle.id);
  gl->deleteTextures(1, (const GLuint*)&nuklear_media.pause.handle.id);
  gl->deleteTextures(1, (const GLuint*)&nuklear_media.stop.handle.id);
  gl->deleteTextures(1, (const GLuint*)&nuklear_media.next.handle.id);
  gl->deleteTextures(1, (const GLuint*)&nuklear_media.prev.handle.id);
  gl->deleteTextures(1, (const GLuint*)&nuklear_media.tools.handle.id);
  gl->deleteTextures(1, (const GLuint*)&nuklear_media.dir.handle.id);
  gl->deleteTextures(1, (const GLuint*)&nuklear_media.del.handle.id);

  nk_font_atlas_clear(&nuklear_state.atlas);
#endif /*  RONA_NUKLEAR_DEMO_WITH_IMAGES   */

  nk_free(&nuklear_state.ctx);

  device_shutdown(game_state->gl, &nuklear_state);
#endif

  level1_shutdown(game_state->level);
  renderer_shutdown(game_state->gl);
}

Vec2 stage_from_window(GameState* game_state, f32 x, f32 y) {
  Vec2 pos;
  pos.x = x;
  pos.y = (f32)game_state->render_struct.window_height - y;

  return vec2_mul(vec2_add(pos, game_state->stage_from_window_delta),
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
  RonaGL*        gl = game_state->gl;
  BumpAllocator* bump_transient = &(game_state->arena_transient);
  Tileset*       tileset = &(game_state->render_struct.tileset);
  RenderStruct*  render_struct = &(game_state->render_struct);

  renderer_lib_load(gl, bump_transient, render_struct);

  stage_from_window_calc(game_state);

  Colour transparent = colour_make(ColourFormat_RGB, 0.0f, 0.0f, 0.0f, 0.0f);
  Colour red = colour_make(ColourFormat_HSLuv, 400.0f, 90.0f, 30.0f, 1.0f);

  mesh_lib_load_single_tile(game_state->mesh_pit, gl, tileset, TS_PressurePadActivated, red,
                            transparent);
  mesh_lib_load_single_tile(game_state->mesh_block, gl, tileset, TS_Block, red, transparent);
  mesh_lib_load_single_tile(game_state->mesh_hero, gl, tileset, TS_Hero, red, transparent);
  mesh_screen_lib_load(game_state->mesh_screen, gl, render_struct);
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
  RenderStruct* render_struct = &(game_state->render_struct);
  RonaGL*       gl = game_state->gl;

  game_state->arena_transient.used = 0;

  if (game_state->window_resized) {
    stage_from_window_calc(game_state);
  }

  text_render_reset(render_struct);

  Vec2        mouse_on_stage = stage_from_window(game_state, (f32)game_state->input->mouse_pos.x,
                                          (f32)game_state->input->mouse_pos.y);
  TextParams* text_params = &(game_state->text_params_debug);
  text_params->pos = vec2(0.0f, render_struct->stage_height - TILE_HEIGHT);
  text_printf(text_params, "%s (%.2f, %.2f)", game_state->mode == GameMode_Edit ? "Edit" : "Play",
              mouse_on_stage.x, mouse_on_stage.y);

  if (key_down(game_state->input, Key_Escape) || key_down(game_state->input, Key_Q)) {
    game_state->quit_game = true;
    return;
  }
  if (key_pressed(game_state->input, Key_M)) {
    game_state->mode = game_state->mode == GameMode_Edit ? GameMode_Play : GameMode_Edit;
  }

#ifdef RONA_NUKLEAR
  if (game_state->mode == GameMode_Edit) {
    struct nk_context* ctx = &nuklear_state.ctx;
    nk_input_begin(ctx);
    int x = (int)game_state->input->mouse_pos.x;
    int y = (int)game_state->input->mouse_pos.y;
    nk_input_motion(ctx, x, y);
    nk_input_button(ctx, NK_BUTTON_LEFT, x, y, mouse_down(game_state->input, MouseButton_Left));
    nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y, mouse_down(game_state->input, MouseButton_Middle));
    nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, mouse_down(game_state->input, MouseButton_Right));
    nk_input_end(ctx);
  }
#endif

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

  renderer_render(game_state);
}
