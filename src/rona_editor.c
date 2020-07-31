
void* nuklear_persistent_alloc(nk_handle h, void* mem, nk_size bytes) {
  void* addr = rona_realloc(&(editor_state.allocator_permanent), mem, bytes);
#if 0
  RONA_LOG("called persistent_alloc %p to %lu bytes -> %p\n", mem, bytes, addr);
#endif
  return addr;
}

void nuklear_persistent_free(nk_handle h, void* mem) {
#if 1
  RONA_LOG("called persistent_free %p\n", mem);
#endif
  rona_free(&(editor_state.allocator_permanent), mem);
}

void* nuklear_transient_alloc(nk_handle h, void* mem, nk_size bytes) {
  if (!editor_state.transient_allocation_calls_expected) {
    RONA_ERROR("transient_free called after expected scope\n");
    return NULL;
  }
  void* addr = rona_realloc(&(editor_state.allocator_transient), mem, bytes);
#if 0
  RONA_LOG("called transient_alloc %p to %lu bytes -> %p\n", mem, bytes, addr);
#endif
  return addr;
}

void nuklear_transient_free(nk_handle h, void* mem) {
  if (!editor_state.transient_allocation_calls_expected) {
    RONA_ERROR("transient_free called after expected scope\n");
    return;
  }
#if 0
  RONA_LOG("called transient_free %p\n", mem);
#endif
  rona_free(&(editor_state.allocator_transient), mem);
}

static void tiny_demo(struct nk_context* ctx) {
  enum { EASY, HARD };
  static int   op = EASY;
  static float value = 0.6f;

  if (nk_begin(ctx, "Show", nk_rect(50, 50, 220, 220),
               NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE)) {
    /* fixed widget pixel width */
    nk_layout_row_static(ctx, 30, 80, 1);
    if (nk_button_label(ctx, "button")) {
      /* event handling */
    }

    /* fixed widget window ratio width */
    nk_layout_row_dynamic(ctx, 30, 2);
    if (nk_option_label(ctx, "easy", op == EASY))
      op = EASY;
    if (nk_option_label(ctx, "hard", op == HARD))
      op = HARD;

    /* custom widget pixel width */
    nk_layout_row_begin(ctx, NK_STATIC, 30, 2);
    {
      nk_layout_row_push(ctx, 50);
      nk_label(ctx, "Volume:", NK_TEXT_LEFT);
      nk_layout_row_push(ctx, 110);
      nk_slider_float(ctx, 0, &value, 1.0f, 0.1f);
    }
    nk_layout_row_end(ctx);
  }
  nk_end(ctx);
}

void tex_demo(struct nk_context* ctx, GLuint stage_texture) {
  if (nk_begin(ctx, "in-game", nk_rect(50, 250, STAGE_WIDTH, STAGE_HEIGHT + 65),
               NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE)) {
    // NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE

    /* custom widget pixel width */
    nk_layout_row_begin(ctx, NK_DYNAMIC, 300, 1);
    {
      // nk_layout_row_push(ctx, 300);
      nk_layout_row_push(ctx, STAGE_WIDTH / STAGE_HEIGHT);

      struct nk_image stage_image = nk_image_id(stage_texture);
      // stage_image.w = STAGE_WIDTH;
      // stage_image.h = STAGE_HEIGHT;

      nk_image(ctx, stage_image);
    }
    nk_layout_row_end(ctx);
  }
  nk_end(ctx);
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
  gl->texImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, image);
}

void editor_startup(RonaGL* gl, EditorState* editor_state, BumpAllocator* permanent,
                    BumpAllocator* transient) {
  // memory management code
  //

  // allocate some memory for nuklear
  editor_state->nuklear_memory_size = megabytes(MEMORY_ALLOCATION_NUKLEAR);
  void* nuklear_memory = (void*)BUMP_ALLOC(permanent, editor_state->nuklear_memory_size);

  // align memory with nk_draw_command
  const nk_size cmd_align = NK_ALIGNOF(struct nk_draw_command);
  u64           align_mask = (cmd_align << 1) - 1;
  void* aligned_nuklear_memory = nuklear_memory - ((u64)nuklear_memory & align_mask) + cmd_align;

  editor_state->nuklear_memory_size -= aligned_nuklear_memory - nuklear_memory;
  editor_state->nuklear_memory = aligned_nuklear_memory;

  // allocate memory for nuklear atlas
  usize nuklear_atlas_memory_size = megabytes(MEMORY_ALLOCATION_NUKLEAR_ATLAS);
  void* nuklear_atlas_memory = (void*)BUMP_ALLOC(permanent, nuklear_atlas_memory_size);

  editor_state->bump_permanent.size = nuklear_atlas_memory_size;
  editor_state->bump_permanent.base = nuklear_atlas_memory;
  editor_state->bump_permanent.used = 0;

  fixed_block_allocator_reset(&(editor_state->allocator_permanent), &(editor_state->bump_permanent));

  // allocating from transient memory as we're only expecting transient allocations to occur during
  // this function
  usize nuklear_atlas_transient_memory_size = megabytes(6);
  void* nuklear_atlas_transient_memory =
      (void*)BUMP_ALLOC(transient, nuklear_atlas_transient_memory_size);

  editor_state->bump_transient.size = nuklear_atlas_transient_memory_size;
  editor_state->bump_transient.base = nuklear_atlas_transient_memory;
  editor_state->bump_transient.used = 0;

  fixed_block_allocator_reset(&(editor_state->allocator_transient), &(editor_state->bump_transient));

  editor_state->persistent.alloc = &nuklear_persistent_alloc;
  editor_state->persistent.free = &nuklear_persistent_free;

  editor_state->transient.alloc = &nuklear_transient_alloc;
  editor_state->transient.free = &nuklear_transient_free;

  // nk_buffer_init_default(&editor_state->cmds);
  nk_buffer_init(&editor_state->cmds, &(editor_state->persistent), NK_BUFFER_DEFAULT_INITIAL_SIZE);

  editor_state->stage_in_nuklear_texture_id = create_texture(gl, STAGE_WIDTH, STAGE_HEIGHT);
  editor_state->depth_texture_id = create_depth_texture(gl, STAGE_WIDTH, STAGE_HEIGHT);
  editor_state->framebuffer_id = create_framebuffer(gl);
  attach_textures_to_framebuffer(gl, editor_state->framebuffer_id,
                                 editor_state->stage_in_nuklear_texture_id,
                                 editor_state->depth_texture_id);
  if (!is_framebuffer_ok(gl)) {
    RONA_ERROR("%d, Nuklear stage Framebuffer is not ok\n", 1);
  }
  gl->bindFramebuffer(GL_FRAMEBUFFER, editor_state->framebuffer_id);
  gl->viewport(0, 0, STAGE_WIDTH, STAGE_HEIGHT);

  // all transient memory operations should happen in-between setting this variable from true to
  // false
  editor_state->transient_allocation_calls_expected = true;

  const void*           image;
  int                   w, h;
  struct nk_font_config cfg = nk_font_config(0);
  cfg.oversample_h = 3;
  cfg.oversample_v = 2;
  /* Loading one font with different heights is only required if you want higher
   * quality text otherwise you can just set the font height directly
   * e.g.: device.ctx->style.font.height = 20. */

  struct nk_font_atlas* atlas = &(editor_state->atlas);

  // call nk_font_atlas_init_custom instead
  nk_font_atlas_init_custom(atlas, &(editor_state->persistent), &(editor_state->transient));
  nk_font_atlas_begin(atlas);

  editor_state->default_font = nk_font_atlas_add_default(atlas, 14.0f, &cfg);

  image = nk_font_atlas_bake(atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
  upload_atlas(gl, editor_state, image, w, h);
  nk_font_atlas_end(atlas, nk_handle_id((int)editor_state->font_tex), &editor_state->null);

  // note: after nk_font_atlas_end is called I'm assuming there won't be any more transient memory
  // allocations
  editor_state->transient_allocation_calls_expected = false;

  nk_init_fixed(&editor_state->ctx, editor_state->nuklear_memory,
                (nk_size)editor_state->nuklear_memory_size, &(editor_state->default_font->handle));
}

void editor_shutdown(RonaGL* gl, EditorState* dev) {
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
  gl->vertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, vs,
                          (void*)vc);

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
      config.vertex_layout = vertex_layout;
      config.vertex_size = sizeof(struct nk_glfw_vertex);
      config.vertex_alignment = NK_ALIGNOF(struct nk_glfw_vertex);
      config.null = editor_state->null;
      config.circle_segment_count = 22;
      config.curve_segment_count = 22;
      config.arc_segment_count = 22;
      config.global_alpha = 1.0f;
      config.shape_AA = AA;
      config.line_AA = AA;

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
