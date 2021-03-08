
void* nuklear_persistent_alloc(nk_handle h, void* mem, nk_size bytes) {
  void* addr = rona_realloc(&(editor_state.fixed_block_permanent), mem, bytes);
#if 0
  rona_log("called persistent_alloc %p to %lu bytes -> %p", mem, bytes, addr);
#endif
  return addr;
}

void nuklear_persistent_free(nk_handle h, void* mem) {
#if 1
  rona_log("called persistent_free %p", mem);
#endif
  rona_free(&(editor_state.fixed_block_permanent), mem);
}

void* nuklear_transient_alloc(nk_handle h, void* mem, nk_size bytes) {
  if (!editor_state.transient_allocation_calls_expected) {
    rona_error("transient_free called after expected scope");
    return NULL;
  }
  void* addr = rona_realloc(&(editor_state.fixed_block_transient), mem, bytes);
#if 0
  rona_log("called transient_alloc %p to %lu bytes -> %p", mem, bytes, addr);
#endif
  return addr;
}

void nuklear_transient_free(nk_handle h, void* mem) {
  if (!editor_state.transient_allocation_calls_expected) {
    rona_error("transient_free called after expected scope");
    return;
  }
#if 0
  rona_log("called transient_free %p", mem);
#endif
  rona_free(&(editor_state.fixed_block_transient), mem);
}

char* rona_sprintf(BumpAllocator* transient, char* fmt, ...) {

  char*   buffer               = (char*)bump_head(transient);
  u64     bump_space_available = transient->size - transient->used;
  va_list va;

  va_start(va, fmt);
  stbsp_vsnprintf(buffer, bump_space_available, fmt, va);
  va_end(va);

  return buffer;
}

void declare_stage_info(EditorState* editor_state, GameState* game_state) {
  BumpAllocator*     transient = &(game_state->bump_transient);
  struct nk_context* ctx       = &(editor_state->ctx);

  Vec2i screen = game_state->input->mouse_pos;
  Vec2i stage  = editor_state->cursor_in_stage_coords;

  if (nk_begin(ctx, "Stage Info", nk_rect(50, 50, 220, 220),
               NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE)) {
    /* fixed widget pixel width */
    nk_layout_row_static(ctx, 30, 120, 1);
    if (nk_button_label(ctx, "Build Walls")) {
      Level* level = game_state->level;
      command_transaction_begin(&editor_state->undo_redo);
      {
        Command* command = command_add(&editor_state->undo_redo, &level->fixed_block_allocator, game_state);

        command->type = CommandType_Editor_WallsBuild;
        // command->params.tile_change.level = level;
        // command->params.tile_change.tile_pos = tile_pos;

        // Tile* tile_old = tile_ensure_get(level, tile_pos);
        // Tile  tile_new;

        // if (editor_state->active_tile_type == 0) {
        //   tile_new.type = TileType_Void;
        //   tile_new.sprite = S_DebugBlank;
        // } else {
        //   tile_new.type = TileType_Floor;
        //   tile_new.sprite = S_Debug4Corners;
        // }

        // command->params.tile_change.tile_old = *tile_old;
        // command->params.tile_change.tile_new = tile_new;
      }
      command_transaction_end(&editor_state->undo_redo);
    }

    nk_layout_row_begin(ctx, NK_DYNAMIC, 30, 2);
    {
      nk_layout_row_push(ctx, 0.5f);
      nk_label(ctx, "Screen Coords:", NK_TEXT_RIGHT);
      nk_layout_row_push(ctx, 0.5f);

      char* b = rona_sprintf(transient, "%d, %d", screen.x, screen.y);
      nk_text(ctx, b, strlen(b), NK_TEXT_LEFT);
    }
    nk_layout_row_end(ctx);

    nk_layout_row_begin(ctx, NK_DYNAMIC, 30, 2);
    {
      nk_layout_row_push(ctx, 0.5f);
      nk_label(ctx, "Stage Coords:", NK_TEXT_RIGHT);
      nk_layout_row_push(ctx, 0.5f);
      char* b = rona_sprintf(transient, "%d, %d", stage.x, stage.y);
      nk_text(ctx, b, strlen(b), NK_TEXT_LEFT);
    }
    nk_layout_row_end(ctx);
  }
  nk_end(ctx);
}

void declare_stage_preview(EditorState* editor_state, GameState* game_state) {
  struct nk_context* ctx           = &(editor_state->ctx);
  GLuint             stage_texture = editor_state->stage_in_nuklear_texture_id;

#define HACK_WINDOW_WIDTH_EXTRA 25
#define HACK_WINDOW_HEIGHT_EXTRA 55

  if (nk_begin(ctx, "Stage Preview",
               nk_rect(50, 250, editor_state->stage_scalar * (STAGE_WIDTH + HACK_WINDOW_WIDTH_EXTRA),
                       editor_state->stage_scalar * (STAGE_HEIGHT + HACK_WINDOW_HEIGHT_EXTRA)),
               NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE)) {
    nk_layout_row_begin(ctx, NK_STATIC, editor_state->stage_scalar * STAGE_HEIGHT, 1);
    {
      nk_layout_row_push(ctx, editor_state->stage_scalar * STAGE_WIDTH);

      struct nk_image stage_image  = nk_image_id(stage_texture);
      struct nk_rect  stage_region = nk_window_get_content_region(ctx);

      // calculate where the cursor is in relation to the stage preview
      Vec2i stage_window                         = vec2i((i32)stage_region.x, (i32)stage_region.y);
      Vec2i stage_window_offset_to_stage_texture = vec2i(0, 4);
      Vec2i screen                               = game_state->input->mouse_pos;
      editor_state->cursor_in_stage_coords =
          vec2i_sub(screen, vec2i_add(stage_window, stage_window_offset_to_stage_texture));
      editor_state->cursor_in_stage_coords.x /= editor_state->stage_scalar;
      editor_state->cursor_in_stage_coords.y /= editor_state->stage_scalar;

      nk_image(ctx, stage_image);
    }
    nk_layout_row_end(ctx);
  }
  nk_end(ctx);
}

void declare_stage_toolbar(EditorState* editor_state, GameState* game_state) {
  struct nk_context* ctx = &(editor_state->ctx);

  if (nk_begin(ctx, "Stage Toolbar", nk_rect(350, 50, 220, 220),
               NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE)) {
    /* fixed widget pixel width */
    nk_layout_row_dynamic(ctx, 30, 2);
    if (nk_button_label(ctx, "undo")) {
      command_undo(&editor_state->undo_redo, game_state);
    }
    if (nk_button_label(ctx, "redo")) {
      command_redo(&editor_state->undo_redo, game_state);
    }

    nk_layout_row_dynamic(ctx, 30, 1);
    // clang-format off
    if (nk_option_label(ctx, "void",  editor_state->active_tile_type == 0)) editor_state->active_tile_type = 0;
    if (nk_option_label(ctx, "floor", editor_state->active_tile_type == 1)) editor_state->active_tile_type = 1;
    // clang-format on
  }
  nk_end(ctx);
}

Sprite tile_default_sprite_for_tiletype(TileType type) {
  Sprite sprite;

  switch (type) {
  case TileType_Void:
    sprite = S_Blank;
    break;
  case TileType_Floor:
    sprite = S_Floor01;
    break;
  default:
    sprite = S_Warning;
    rona_error("unknown TileType: %d", type);
  }

  return sprite;
}

Tile tile_placement_manual(TileType type) {
  Tile tile;

  tile.type             = type;
  tile.sprite           = tile_default_sprite_for_tiletype(type);
  tile.placement_reason = TilePlacementReason_Manual;

  return tile;
}

// given two Vec2i inputs returns them sorted into top-left and bottom-right vectors
//
void vec2i_as_top_left_bottom_right(Vec2i* tl, Vec2i* br, Vec2i in1, Vec2i in2) {
  tl->x = in1.x < in2.x ? in1.x : in2.x;
  tl->y = in1.y < in2.y ? in1.y : in2.y;
  br->x = in1.x > in2.x ? in1.x : in2.x;
  br->y = in1.y > in2.y ? in1.y : in2.y;
}

void editor_step(EditorState* editor_state, GameState* game_state) {
  declare_stage_preview(editor_state, game_state);
  declare_stage_info(editor_state, game_state);
  declare_stage_toolbar(editor_state, game_state);

  if (mouse_pressed(game_state->input, MouseButton_Left)) {
    if (rect_contains_point(rect(0, 0, STAGE_WIDTH, STAGE_HEIGHT), editor_state->cursor_in_stage_coords)) {
      editor_state->mouse_down_in_stage_coords = editor_state->cursor_in_stage_coords;
      editor_state->mouse_down_valid           = true;
    } else {
      editor_state->mouse_down_valid = false; // not part of a drag operation
    }
  }
  if (mouse_released(game_state->input, MouseButton_Left)) {
    if (rect_contains_point(rect(0, 0, STAGE_WIDTH, STAGE_HEIGHT), editor_state->cursor_in_stage_coords)) {
      editor_state->mouse_up_in_stage_coords = editor_state->cursor_in_stage_coords;
      editor_state->mouse_up_valid           = true;
    } else {
      editor_state->mouse_up_valid = false; // not part of a drag operation
    }

    if (editor_state->mouse_down_valid && editor_state->mouse_up_valid) {
      Level* level = game_state->level;

      // a mouse drag and release operation has been performed
      //
      if (vec2i_eq(editor_state->mouse_down_in_stage_coords, editor_state->mouse_up_in_stage_coords)) {
        // a single tile is being added
        //
        TilePos tile_pos =
            tile_pos_from_stage_coords(editor_state->cursor_in_stage_coords, level->viewport.pos);
        Tile* tile_current = tile_ensure_get(level, tile_pos);
        Tile  tile_new     = tile_placement_manual(editor_state->active_tile_type);

        if (!tile_eq(*tile_current, tile_new)) {
          // build and execute the command to add a tile (using the TileChange command as it's lighter)
          //
          command_transaction_begin(&editor_state->undo_redo);
          {
            Command* command =
                command_add(&editor_state->undo_redo, &level->fixed_block_allocator, game_state);

            command->type                        = CommandType_Editor_TileChange;
            command->params.tile_change.tile_pos = tile_pos;
            command->params.tile_change.tile_old = *tile_current;
            command->params.tile_change.tile_new = tile_new;

            // perform the actual command
            command_execute(command, CommandExecute_Redo, game_state);
          }
          command_transaction_end(&editor_state->undo_redo);
        }
      } else {
        // multiple tiles are being added (have dragged an area)
        //
        Vec2i stage_tl, stage_br;
        vec2i_as_top_left_bottom_right(&stage_tl, &stage_br, editor_state->mouse_down_in_stage_coords,
                                       editor_state->mouse_up_in_stage_coords);

        Tile tile_new           = tile_placement_manual(editor_state->active_tile_type);
        i32  num_modified_tiles = 0;
        i32  num_tiles          = 0;

        Vec2i world_tl = tile_world_space_from_stage_coords(stage_tl, level->viewport.pos);
        Vec2i world_br = tile_world_space_from_stage_coords(stage_br, level->viewport.pos);

        for (i32 y = world_tl.y; y <= world_br.y; y++) {
          for (i32 x = world_tl.x; x <= world_br.x; x++) {
            TilePos tile_pos     = tile_pos_from_world_tile_space(vec2i(x, y));
            Tile*   tile_current = tile_ensure_get(level, tile_pos);

            if (!tile_eq(*tile_current, tile_new)) {
              num_modified_tiles++;
            }
            num_tiles++;
          }
        }
        if (num_modified_tiles > 0) {
          command_transaction_begin(&editor_state->undo_redo);
          {
            Command* command =
                command_add(&editor_state->undo_redo, &level->fixed_block_allocator, game_state);

            command->type                          = CommandType_Editor_TileArea;
            EditorCommandParamsTileArea* tile_area = &(command->params.tile_area);

            tile_area->tile_world_pos_top_left     = world_tl;
            tile_area->tile_world_pos_bottom_right = world_br;

            // Tile* tile_old = tile_ensure_get(level, tile_pos);
            tile_area->tile_old_sb = NULL;
            Tile* foo = sb_add(&editor_state->fixed_block_permanent, tile_area->tile_old_sb, num_tiles);
            RONA_ASSERT(foo);

            i32 tile_num = 0;
            for (i32 y = world_tl.y; y <= world_br.y; y++) {
              for (i32 x = world_tl.x; x <= world_br.x; x++) {
                TilePos tile_pos     = tile_pos_from_world_tile_space(vec2i(x, y));
                Tile*   tile_current = tile_ensure_get(level, tile_pos);

                tile_area->tile_old_sb[tile_num] = *tile_current;

                tile_num++;
              }
            }

            tile_area->tile_new = tile_new;

            // perform the actual command
            command_execute(command, CommandExecute_Redo, game_state);
          }
          command_transaction_end(&editor_state->undo_redo);
        }
      }
    }
  }
}

struct nk_glfw_vertex {
  float   position[2];
  float   uv[2];
  nk_byte col[4];
};

void upload_atlas(RonaGL* gl, EditorState* dev, const void* image, int width, int height) {
  gl->genTextures(1, &dev->font_tex);
  gl->bindTexture(GL_TEXTURE_2D, dev->font_tex);
  gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  gl->texImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 image);
}

void editor_startup(RonaGL* gl, EditorState* editor_state, BumpAllocator* permanent,
                    BumpAllocator* transient) {
  // memory management code
  //

  // allocate some memory for nuklear
  editor_state->nuklear_memory_size = megabytes(MEMORY_ALLOCATION_NUKLEAR);
  void* nuklear_memory              = (void*)BUMP_ALLOC(permanent, editor_state->nuklear_memory_size);

  // align memory with nk_draw_command
  const nk_size cmd_align              = NK_ALIGNOF(struct nk_draw_command);
  u64           align_mask             = (cmd_align << 1) - 1;
  void*         aligned_nuklear_memory = nuklear_memory - ((u64)nuklear_memory & align_mask) + cmd_align;

  editor_state->nuklear_memory_size -= aligned_nuklear_memory - nuklear_memory;
  editor_state->nuklear_memory = aligned_nuklear_memory;

  // allocate memory for nuklear atlas
  usize nuklear_atlas_memory_size = megabytes(MEMORY_ALLOCATION_EDITOR);
  void* nuklear_atlas_memory      = (void*)BUMP_ALLOC(permanent, nuklear_atlas_memory_size);
  // should the atlas memory also be aligned???

  // editor_state->bump_permanent.size = nuklear_atlas_memory_size;
  // editor_state->bump_permanent.base = nuklear_atlas_memory;
  // editor_state->bump_permanent.used = 0;
  bump_allocator_reset(&(editor_state->bump_permanent), nuklear_atlas_memory, nuklear_atlas_memory_size, true,
                       "Editor Permanent");

  fixed_block_allocator_reset(&(editor_state->fixed_block_permanent), &(editor_state->bump_permanent), false,
                              NULL);

  // allocating from transient memory as we're only expecting transient allocations to occur during
  // this function
  usize nuklear_atlas_transient_memory_size = megabytes(6);
  void* nuklear_atlas_transient_memory = (void*)BUMP_ALLOC(transient, nuklear_atlas_transient_memory_size);

  editor_state->bump_transient.size = nuklear_atlas_transient_memory_size;
  editor_state->bump_transient.base = nuklear_atlas_transient_memory;
  editor_state->bump_transient.used = 0;

  fixed_block_allocator_reset(&(editor_state->fixed_block_transient), &(editor_state->bump_transient), false,
                              NULL);

  struct nk_allocator nk_fixed_block_permanent;
  nk_fixed_block_permanent.alloc = &nuklear_persistent_alloc;
  nk_fixed_block_permanent.free  = &nuklear_persistent_free;

  nk_buffer_init(&editor_state->cmds, &nk_fixed_block_permanent, NK_BUFFER_DEFAULT_INITIAL_SIZE);

  editor_state->stage_in_nuklear_texture_id = create_texture(gl, STAGE_WIDTH, STAGE_HEIGHT);
  editor_state->depth_texture_id            = create_depth_texture(gl, STAGE_WIDTH, STAGE_HEIGHT);
  editor_state->framebuffer_id              = create_framebuffer(gl);
  attach_textures_to_framebuffer(gl, editor_state->framebuffer_id, editor_state->stage_in_nuklear_texture_id,
                                 editor_state->depth_texture_id);
  if (!is_framebuffer_ok(gl)) {
    rona_error("%d, Nuklear stage Framebuffer is not ok", 1);
  }
  gl->bindFramebuffer(GL_FRAMEBUFFER, editor_state->framebuffer_id);
  gl->viewport(0, 0, STAGE_WIDTH, STAGE_HEIGHT);

  // all transient memory operations should happen in-between setting this variable from true to
  // false
  editor_state->transient_allocation_calls_expected = true;

  struct nk_allocator nk_fixed_block_transient;
  nk_fixed_block_transient.alloc = &nuklear_transient_alloc;
  nk_fixed_block_transient.free  = &nuklear_transient_free;

  const void*           image;
  int                   w, h;
  struct nk_font_config cfg = nk_font_config(0);
  cfg.oversample_h          = 3;
  cfg.oversample_v          = 2;
  /* Loading one font with different heights is only required if you want higher
   * quality text otherwise you can just set the font height directly
   * e.g.: device.ctx->style.font.height = 20. */

  struct nk_font_atlas* atlas = &(editor_state->atlas);

  // call nk_font_atlas_init_custom instead
  nk_font_atlas_init_custom(atlas, &nk_fixed_block_permanent, &nk_fixed_block_transient);
  nk_font_atlas_begin(atlas);

  editor_state->default_font = nk_font_atlas_add_default(atlas, 14.0f, &cfg);

  image = nk_font_atlas_bake(atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
  upload_atlas(gl, editor_state, image, w, h);
  nk_font_atlas_end(atlas, nk_handle_id((int)editor_state->font_tex), &editor_state->null);

  // note: after nk_font_atlas_end is called I'm assuming there won't be any more transient memory
  // allocations
  editor_state->transient_allocation_calls_expected = false;

  nk_init_fixed(&editor_state->ctx, editor_state->nuklear_memory, (nk_size)editor_state->nuklear_memory_size,
                &(editor_state->default_font->handle));

  editor_state->stage_scalar = 2;
}

void editor_shutdown(RonaGL* gl, EditorState* dev) {
  command_system_shutdown(&(dev->undo_redo));

  nk_free(&dev->ctx);

  gl->deleteTextures(1, &dev->font_tex);
  nk_buffer_free(&dev->cmds);
}

void editor_lib_load(RonaGL* gl, EditorState* editor_state, ShaderEditor* shader_editor) {
  /* buffer setup */
  GLsizei vs = sizeof(struct nk_glfw_vertex);
  size_t  vp = offsetof(struct nk_glfw_vertex, position);
  size_t  vt = offsetof(struct nk_glfw_vertex, uv);
  size_t  vc = offsetof(struct nk_glfw_vertex, col);

  gl->genBuffers(1, &editor_state->vbo);
  gl->genBuffers(1, &editor_state->ebo);
  gl->genVertexArrays(1, &editor_state->vao);

  gl->bindVertexArray(editor_state->vao);
  gl->bindBuffer(GL_ARRAY_BUFFER, editor_state->vbo);
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, editor_state->ebo);

  gl->enableVertexAttribArray(0);
  gl->enableVertexAttribArray(1);
  gl->enableVertexAttribArray(2);

  gl->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, vs, (void*)vp);
  gl->vertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vs, (void*)vt);
  gl->vertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, vs, (void*)vc);

  gl->bindTexture(GL_TEXTURE_2D, 0);
  gl->bindBuffer(GL_ARRAY_BUFFER, 0);
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  gl->bindVertexArray(0);
}

void editor_lib_unload(RonaGL* gl, EditorState* editor_state) {
  gl->deleteBuffers(1, &editor_state->vbo);
  gl->deleteBuffers(1, &editor_state->ebo);

  gl->deleteVertexArrays(1, &editor_state->vao);
}

void editor_changed_level(EditorState* editor_state, Level* level) {
  RONA_ASSERT(editor_state);
  RONA_ASSERT(level);

  // use the level's bump allocator to store undo/redo information
  if (!command_system_startup(&(editor_state->undo_redo), &(level->fixed_block_allocator),
                              RESERVE_NUM_COMMANDS_IN_UNDO_BUFFER)) {
    rona_error("editor_startup: command_system_startup failed");
  }
}

void editor_render(RonaGL* gl, EditorState* editor_state, int width, int height,
                   ShaderEditor* shader_editor) {

  enum nk_anti_aliasing AA = NK_ANTI_ALIASING_ON;

  struct nk_context* ctx = &editor_state->ctx;

  struct nk_vec2 scale;
  scale.x = 1.0f;
  scale.y = 1.0f;

  GLfloat ortho[4][4] = {
      {2.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, -2.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, -1.0f, 0.0f},
      {-1.0f, 1.0f, 0.0f, 1.0f},
  };
  ortho[0][0] /= (GLfloat)width;
  ortho[1][1] /= (GLfloat)height;

  /* setup global state */
  gl->enable(GL_BLEND);
  gl->blendEquation(GL_FUNC_ADD);
  gl->blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  gl->disable(GL_CULL_FACE);
  gl->disable(GL_DEPTH_TEST);
  gl->enable(GL_SCISSOR_TEST);
  gl->activeTexture(GL_TEXTURE0);

  /* setup program */
  gl->useProgram(shader_editor->program);
  gl->uniform1i(shader_editor->uniform_texture, 0);
  gl->uniformMatrix4fv(shader_editor->uniform_proj_matrix, 1, GL_FALSE, &ortho[0][0]);
  {
    /* convert from command queue into draw list and draw to screen */
    const struct nk_draw_command* cmd;
    void *                        vertices, *elements;
    const nk_draw_index*          offset = NULL;

    /* allocate vertex and element buffer */
    gl->bindVertexArray(editor_state->vao);
    gl->bindBuffer(GL_ARRAY_BUFFER, editor_state->vbo);
    gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, editor_state->ebo);

    gl->bufferData(GL_ARRAY_BUFFER, MAX_VERTEX_MEMORY, NULL, GL_STREAM_DRAW);
    gl->bufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_ELEMENT_MEMORY, NULL, GL_STREAM_DRAW);

    /* load draw vertices & elements directly into vertex + element buffer */
    vertices = gl->mapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    elements = gl->mapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
    {
      /* fill convert configuration */
      struct nk_convert_config                          config;
      static const struct nk_draw_vertex_layout_element vertex_layout[] = {
          {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_glfw_vertex, position)},
          {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_glfw_vertex, uv)},
          {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(struct nk_glfw_vertex, col)},
          {NK_VERTEX_LAYOUT_END}};
      NK_MEMSET(&config, 0, sizeof(config));
      config.vertex_layout        = vertex_layout;
      config.vertex_size          = sizeof(struct nk_glfw_vertex);
      config.vertex_alignment     = NK_ALIGNOF(struct nk_glfw_vertex);
      config.null                 = editor_state->null;
      config.circle_segment_count = 22;
      config.curve_segment_count  = 22;
      config.arc_segment_count    = 22;
      config.global_alpha         = 1.0f;
      config.shape_AA             = AA;
      config.line_AA              = AA;

      /* setup buffers to load vertices and elements */
      {
        struct nk_buffer vbuf, ebuf;
        nk_buffer_init_fixed(&vbuf, vertices, MAX_VERTEX_MEMORY);
        nk_buffer_init_fixed(&ebuf, elements, MAX_ELEMENT_MEMORY);
        nk_convert(ctx, &editor_state->cmds, &vbuf, &ebuf, &config);
      }
    }
    gl->unmapBuffer(GL_ARRAY_BUFFER);
    gl->unmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    /* iterate over and execute each draw command */
    nk_draw_foreach(cmd, ctx, &editor_state->cmds) {
      if (!cmd->elem_count)
        continue;
      gl->bindTexture(GL_TEXTURE_2D, (GLuint)cmd->texture.id);
      gl->scissor((GLint)(cmd->clip_rect.x * scale.x),
                  (GLint)((height - (GLint)(cmd->clip_rect.y + cmd->clip_rect.h)) * scale.y),
                  (GLint)(cmd->clip_rect.w * scale.x), (GLint)(cmd->clip_rect.h * scale.y));
      gl->drawElements(GL_TRIANGLES, (GLsizei)cmd->elem_count, GL_UNSIGNED_SHORT, offset);
      offset += cmd->elem_count;
    }
    nk_clear(ctx);
    nk_buffer_clear(&editor_state->cmds);
  }

  /* default OpenGL state */
  gl->useProgram(0);
  gl->bindBuffer(GL_ARRAY_BUFFER, 0);
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  gl->bindVertexArray(0);
  gl->disable(GL_BLEND);
  gl->disable(GL_SCISSOR_TEST);
}
