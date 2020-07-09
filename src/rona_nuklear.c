void *nuklear_persistent_alloc(nk_handle h, void *mem, nk_size bytes) {
  void *addr = rona_realloc(&(nuklear_state.allocator_permanent), mem, bytes);
#if 0
  RONA_LOG("called persistent_alloc %p to %lu bytes -> %p\n", mem, bytes, addr);
#endif
  return addr;
}

void nuklear_persistent_free(nk_handle h, void *mem) {
#if 1
  RONA_LOG("called persistent_free %p\n", mem);
#endif
  rona_free(&(nuklear_state.allocator_permanent), mem);
}

void *nuklear_transient_alloc(nk_handle h, void *mem, nk_size bytes) {
  if (!nuklear_state.transient_allocation_calls_expected) {
    RONA_ERROR("transient_free called after expected scope\n");
    return NULL;
  }
  void *addr = rona_realloc(&(nuklear_state.allocator_transient), mem, bytes);
#if 0
  RONA_LOG("called transient_alloc %p to %lu bytes -> %p\n", mem, bytes, addr);
#endif
  return addr;
}

void nuklear_transient_free(nk_handle h, void *mem) {
  if (!nuklear_state.transient_allocation_calls_expected) {
    RONA_ERROR("transient_free called after expected scope\n");
    return;
  }
#if 0
  RONA_LOG("called transient_free %p\n", mem);
#endif
  rona_free(&(nuklear_state.allocator_transient), mem);
}


#ifdef RONA_NUKLEAR_DEMO_WITH_IMAGES

/* ===============================================================
 *
 *                          CUSTOM WIDGET
 *
 * ===============================================================*/
static int ui_piemenu(struct nk_context* ctx, struct nk_vec2 pos, float radius,
                      struct nk_image* icons, int item_count) {
  int            ret = -1;
  struct nk_rect total_space;
  struct nk_rect bounds;
  int            active_item = 0;

  /* pie menu popup */
  struct nk_color      border = ctx->style.window.border_color;
  struct nk_style_item background = ctx->style.window.fixed_background;
  ctx->style.window.fixed_background = nk_style_item_hide();
  ctx->style.window.border_color = nk_rgba(0, 0, 0, 0);

  total_space = nk_window_get_content_region(ctx);
  ctx->style.window.spacing = nk_vec2(0, 0);
  ctx->style.window.padding = nk_vec2(0, 0);

  if (nk_popup_begin(ctx, NK_POPUP_STATIC, "piemenu", NK_WINDOW_NO_SCROLLBAR,
                     nk_rect(pos.x - total_space.x - radius, pos.y - radius - total_space.y,
                             2 * radius, 2 * radius))) {
    int                       i = 0;
    struct nk_command_buffer* out = nk_window_get_canvas(ctx);
    const struct nk_input*    in = &ctx->input;

    total_space = nk_window_get_content_region(ctx);
    ctx->style.window.spacing = nk_vec2(4, 4);
    ctx->style.window.padding = nk_vec2(8, 8);
    nk_layout_row_dynamic(ctx, total_space.h, 1);
    nk_widget(&bounds, ctx);

    /* outer circle */
    nk_fill_circle(out, bounds, nk_rgb(50, 50, 50));
    {
      /* circle buttons */
      float step = (2 * 3.141592654f) / (float)(MAX(1, item_count));
      float a_min = 0;
      float a_max = step;

      struct nk_vec2 center = nk_vec2(bounds.x + bounds.w / 2.0f, bounds.y + bounds.h / 2.0f);
      struct nk_vec2 drag = nk_vec2(in->mouse.pos.x - center.x, in->mouse.pos.y - center.y);
      float          angle = (float)atan2(drag.y, drag.x);
      if (angle < -0.0f)
        angle += 2.0f * 3.141592654f;
      active_item = (int)(angle / step);

      for (i = 0; i < item_count; ++i) {
        struct nk_rect content;
        float          rx, ry, dx, dy, a;
        nk_fill_arc(out, center.x, center.y, (bounds.w / 2.0f), a_min, a_max,
                    (active_item == i) ? nk_rgb(45, 100, 255) : nk_rgb(60, 60, 60));

        /* separator line */
        rx = bounds.w / 2.0f;
        ry = 0;
        dx = rx * (float)cos(a_min) - ry * (float)sin(a_min);
        dy = rx * (float)sin(a_min) + ry * (float)cos(a_min);
        nk_stroke_line(out, center.x, center.y, center.x + dx, center.y + dy, 1.0f,
                       nk_rgb(50, 50, 50));

        /* button content */
        a = a_min + (a_max - a_min) / 2.0f;
        rx = bounds.w / 2.5f;
        ry = 0;
        content.w = 30;
        content.h = 30;
        content.x = center.x + ((rx * (float)cos(a) - ry * (float)sin(a)) - content.w / 2.0f);
        content.y = center.y + (rx * (float)sin(a) + ry * (float)cos(a) - content.h / 2.0f);
        nk_draw_image(out, content, &icons[i], nk_rgb(255, 255, 255));
        a_min = a_max;
        a_max += step;
      }
    }
    {
      /* inner circle */
      struct nk_rect inner;
      inner.x = bounds.x + bounds.w / 2 - bounds.w / 4;
      inner.y = bounds.y + bounds.h / 2 - bounds.h / 4;
      inner.w = bounds.w / 2;
      inner.h = bounds.h / 2;
      nk_fill_circle(out, inner, nk_rgb(45, 45, 45));

      /* active icon content */
      bounds.w = inner.w / 2.0f;
      bounds.h = inner.h / 2.0f;
      bounds.x = inner.x + inner.w / 2 - bounds.w / 2;
      bounds.y = inner.y + inner.h / 2 - bounds.h / 2;
      nk_draw_image(out, bounds, &icons[active_item], nk_rgb(255, 255, 255));
    }
    nk_layout_space_end(ctx);
    if (!nk_input_is_mouse_down(&ctx->input, NK_BUTTON_RIGHT)) {
      nk_popup_close(ctx);
      ret = active_item;
    }
  } else
    ret = -2;
  ctx->style.window.spacing = nk_vec2(4, 4);
  ctx->style.window.padding = nk_vec2(8, 8);
  nk_popup_end(ctx);

  ctx->style.window.fixed_background = background;
  ctx->style.window.border_color = border;
  return ret;
}

/* ===============================================================
 *
 *                          BUTTON DEMO
 *
 * ===============================================================*/
static void ui_header(struct nk_context* ctx, NuklearMedia* media, const char* title) {
  nk_style_set_font(ctx, &media->font_18->handle);
  nk_layout_row_dynamic(ctx, 20, 1);
  nk_label(ctx, title, NK_TEXT_LEFT);
}

static void ui_widget(struct nk_context* ctx, NuklearMedia* media, float height) {
  static const float ratio[] = {0.15f, 0.85f};
  nk_style_set_font(ctx, &media->font_22->handle);
  nk_layout_row(ctx, NK_DYNAMIC, height, 2, ratio);
  nk_spacing(ctx, 1);
}

static void ui_widget_centered(struct nk_context* ctx, NuklearMedia* media, float height) {
  static const float ratio[] = {0.15f, 0.50f, 0.35f};
  nk_style_set_font(ctx, &media->font_22->handle);
  nk_layout_row(ctx, NK_DYNAMIC, height, 3, ratio);
  nk_spacing(ctx, 1);
}

static void button_demo(struct nk_context* ctx, NuklearMedia* media) {
  static int option = 1;
  static int toggle0 = 1;
  static int toggle1 = 0;
  static int toggle2 = 1;

  nk_style_set_font(ctx, &media->font_20->handle);
  nk_begin(ctx, "Button Demo", nk_rect(50, 50, 255, 610),
           NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE);

  /*------------------------------------------------
   *                  MENU
   *------------------------------------------------*/
  nk_menubar_begin(ctx);
  {
    /* toolbar */
    nk_layout_row_static(ctx, 40, 40, 4);
    if (nk_menu_begin_image(ctx, "Music", media->play, nk_vec2(110, 120))) {
      /* settings */
      nk_layout_row_dynamic(ctx, 25, 1);
      nk_menu_item_image_label(ctx, media->play, "Play", NK_TEXT_RIGHT);
      nk_menu_item_image_label(ctx, media->stop, "Stop", NK_TEXT_RIGHT);
      nk_menu_item_image_label(ctx, media->pause, "Pause", NK_TEXT_RIGHT);
      nk_menu_item_image_label(ctx, media->next, "Next", NK_TEXT_RIGHT);
      nk_menu_item_image_label(ctx, media->prev, "Prev", NK_TEXT_RIGHT);
      nk_menu_end(ctx);
    }
    nk_button_image(ctx, media->tools);
    nk_button_image(ctx, media->cloud);
    nk_button_image(ctx, media->pen);
  }
  nk_menubar_end(ctx);

  /*------------------------------------------------
   *                  BUTTON
   *------------------------------------------------*/
  ui_header(ctx, media, "Push buttons");
  ui_widget(ctx, media, 35);
  if (nk_button_label(ctx, "Push me"))
    fprintf(stdout, "pushed!\n");
  ui_widget(ctx, media, 35);
  if (nk_button_image_label(ctx, media->rocket, "Styled", NK_TEXT_CENTERED))
    fprintf(stdout, "rocket!\n");

  /*------------------------------------------------
   *                  REPEATER
   *------------------------------------------------*/
  ui_header(ctx, media, "Repeater");
  ui_widget(ctx, media, 35);
  if (nk_button_label(ctx, "Press me"))
    fprintf(stdout, "pressed!\n");

  /*------------------------------------------------
   *                  TOGGLE
   *------------------------------------------------*/
  ui_header(ctx, media, "Toggle buttons");
  ui_widget(ctx, media, 35);
  if (nk_button_image_label(ctx, (toggle0) ? media->checked : media->unchecked, "Toggle",
                            NK_TEXT_LEFT))
    toggle0 = !toggle0;

  ui_widget(ctx, media, 35);
  if (nk_button_image_label(ctx, (toggle1) ? media->checked : media->unchecked, "Toggle",
                            NK_TEXT_LEFT))
    toggle1 = !toggle1;

  ui_widget(ctx, media, 35);
  if (nk_button_image_label(ctx, (toggle2) ? media->checked : media->unchecked, "Toggle",
                            NK_TEXT_LEFT))
    toggle2 = !toggle2;

  /*------------------------------------------------
   *                  RADIO
   *------------------------------------------------*/
  ui_header(ctx, media, "Radio buttons");
  ui_widget(ctx, media, 35);
  if (nk_button_symbol_label(ctx, (option == 0) ? NK_SYMBOL_CIRCLE_OUTLINE : NK_SYMBOL_CIRCLE_SOLID,
                             "Select", NK_TEXT_LEFT))
    option = 0;
  ui_widget(ctx, media, 35);
  if (nk_button_symbol_label(ctx, (option == 1) ? NK_SYMBOL_CIRCLE_OUTLINE : NK_SYMBOL_CIRCLE_SOLID,
                             "Select", NK_TEXT_LEFT))
    option = 1;
  ui_widget(ctx, media, 35);
  if (nk_button_symbol_label(ctx, (option == 2) ? NK_SYMBOL_CIRCLE_OUTLINE : NK_SYMBOL_CIRCLE_SOLID,
                             "Select", NK_TEXT_LEFT))
    option = 2;

  /*------------------------------------------------
   *                  CONTEXTUAL
   *------------------------------------------------*/
  nk_style_set_font(ctx, &media->font_18->handle);
  if (nk_contextual_begin(ctx, NK_WINDOW_NO_SCROLLBAR, nk_vec2(150, 300),
                          nk_window_get_bounds(ctx))) {
    nk_layout_row_dynamic(ctx, 30, 1);
    if (nk_contextual_item_image_label(ctx, media->copy, "Clone", NK_TEXT_RIGHT))
      fprintf(stdout, "pressed clone!\n");
    if (nk_contextual_item_image_label(ctx, media->del, "Delete", NK_TEXT_RIGHT))
      fprintf(stdout, "pressed delete!\n");
    if (nk_contextual_item_image_label(ctx, media->convert, "Convert", NK_TEXT_RIGHT))
      fprintf(stdout, "pressed convert!\n");
    if (nk_contextual_item_image_label(ctx, media->edit, "Edit", NK_TEXT_RIGHT))
      fprintf(stdout, "pressed edit!\n");
    nk_contextual_end(ctx);
  }
  nk_style_set_font(ctx, &media->font_14->handle);
  nk_end(ctx);
}

/* ===============================================================
 *
 *                          BASIC DEMO
 *
 * ===============================================================*/
static void basic_demo(struct nk_context* ctx, NuklearMedia* media) {
  static int            image_active;
  static int            check0 = 1;
  static int            check1 = 0;
  static size_t         prog = 80;
  static int            selected_item = 0;
  static int            selected_image = 3;
  static int            selected_icon = 0;
  static const char*    items[] = {"Item 0", "item 1", "item 2"};
  static int            piemenu_active = 0;
  static struct nk_vec2 piemenu_pos;

  int i = 0;
  nk_style_set_font(ctx, &media->font_20->handle);
  nk_begin(ctx, "Basic Demo", nk_rect(320, 50, 275, 610),
           NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE);

  /*------------------------------------------------
   *                  POPUP BUTTON
   *------------------------------------------------*/
  ui_header(ctx, media, "Popup & Scrollbar & Images");
  ui_widget(ctx, media, 35);
  if (nk_button_image_label(ctx, media->dir, "Images", NK_TEXT_CENTERED))
    image_active = !image_active;

  /*------------------------------------------------
   *                  SELECTED IMAGE
   *------------------------------------------------*/
  ui_header(ctx, media, "Selected Image");
  ui_widget_centered(ctx, media, 100);
  nk_image(ctx, media->images[selected_image]);

  /*------------------------------------------------
   *                  IMAGE POPUP
   *------------------------------------------------*/
  if (image_active) {
    if (nk_popup_begin(ctx, NK_POPUP_STATIC, "Image Popup", 0, nk_rect(265, 0, 320, 220))) {
      nk_layout_row_static(ctx, 82, 82, 3);
      for (i = 0; i < 9; ++i) {
        if (nk_button_image(ctx, media->images[i])) {
          selected_image = i;
          image_active = 0;
          nk_popup_close(ctx);
        }
      }
      nk_popup_end(ctx);
    }
  }
  /*------------------------------------------------
   *                  COMBOBOX
   *------------------------------------------------*/
  ui_header(ctx, media, "Combo box");
  ui_widget(ctx, media, 40);
  if (nk_combo_begin_label(ctx, items[selected_item], nk_vec2(nk_widget_width(ctx), 200))) {
    nk_layout_row_dynamic(ctx, 35, 1);
    for (i = 0; i < 3; ++i)
      if (nk_combo_item_label(ctx, items[i], NK_TEXT_LEFT))
        selected_item = i;
    nk_combo_end(ctx);
  }

  ui_widget(ctx, media, 40);
  if (nk_combo_begin_image_label(ctx, items[selected_icon], media->images[selected_icon],
                                 nk_vec2(nk_widget_width(ctx), 200))) {
    nk_layout_row_dynamic(ctx, 35, 1);
    for (i = 0; i < 3; ++i)
      if (nk_combo_item_image_label(ctx, media->images[i], items[i], NK_TEXT_RIGHT))
        selected_icon = i;
    nk_combo_end(ctx);
  }

  /*------------------------------------------------
   *                  CHECKBOX
   *------------------------------------------------*/
  ui_header(ctx, media, "Checkbox");
  ui_widget(ctx, media, 30);
  nk_checkbox_label(ctx, "Flag 1", &check0);
  ui_widget(ctx, media, 30);
  nk_checkbox_label(ctx, "Flag 2", &check1);

  /*------------------------------------------------
   *                  PROGRESSBAR
   *------------------------------------------------*/
  ui_header(ctx, media, "Progressbar");
  ui_widget(ctx, media, 35);
  nk_progress(ctx, &prog, 100, nk_true);

  /*------------------------------------------------
   *                  PIEMENU
   *------------------------------------------------*/
  if (nk_input_is_mouse_click_down_in_rect(&ctx->input, NK_BUTTON_RIGHT, nk_window_get_bounds(ctx),
                                           nk_true)) {
    piemenu_pos = ctx->input.mouse.pos;
    piemenu_active = 1;
  }

  if (piemenu_active) {
    int ret = ui_piemenu(ctx, piemenu_pos, 140, &media->menu[0], 6);
    if (ret == -2)
      piemenu_active = 0;
    if (ret != -1) {
      fprintf(stdout, "piemenu selected: %d\n", ret);
      piemenu_active = 0;
    }
  }
  nk_style_set_font(ctx, &media->font_14->handle);
  nk_end(ctx);
}

#endif /* RONA_NUKLEAR_DEMO_WITH_IMAGES */

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

static void tex_demo(struct nk_context* ctx, GLuint stage_texture) {
  if (nk_begin(ctx, "in-game", nk_rect(50, 250, STAGE_WIDTH, STAGE_HEIGHT + 65),
               NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE)) {
    // NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE

    /* custom widget pixel width */
    nk_layout_row_begin(ctx, NK_DYNAMIC, 300, 1);
    {
      // nk_layout_row_push(ctx, 300);
      nk_layout_row_push(ctx, STAGE_WIDTH/STAGE_HEIGHT);

      struct nk_image stage_image = nk_image_id(stage_texture);
      // stage_image.w = STAGE_WIDTH;
      // stage_image.h = STAGE_HEIGHT;

      nk_image(ctx, stage_image);
    }
    nk_layout_row_end(ctx);

  }
  nk_end(ctx);
}

/* ===============================================================
 *
 *                          DEVICE
 *
 * ===============================================================*/
struct nk_glfw_vertex {
  float   position[2];
  float   uv[2];
  nk_byte col[4];
};

#ifdef RONA_NUKLEAR_DEMO_WITH_IMAGES
static struct nk_image icon_load(RonaGL* gl, const char* filename) {
  int            x, y, n;
  GLuint         tex;
  unsigned char* data = stbi_load(filename, &x, &y, &n, 0);
  if (!data)
    RONA_ERROR("[nuklear_example]: failed to load image: %s", filename);

  gl->genTextures(1, &tex);
  gl->bindTexture(GL_TEXTURE_2D, tex);
  gl->texParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  gl->texParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  gl->texParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  gl->texParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  gl->texImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  gl->generateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
  return nk_image_id((int)tex);
}
#endif /*  RONA_NUKLEAR_DEMO_WITH_IMAGES  */

static void device_init(RonaGL* gl, NuklearState* dev) {
  GLint                status;
  static const GLchar* vertex_shader =
      NK_SHADER_VERSION "uniform mat4 ProjMtx;\n"
                        "in vec2 Position;\n"
                        "in vec2 TexCoord;\n"
                        "in vec4 Color;\n"
                        "out vec2 Frag_UV;\n"
                        "out vec4 Frag_Color;\n"
                        "void main() {\n"
                        "   Frag_UV = TexCoord;\n"
                        "   Frag_Color = Color;\n"
                        "   gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
                        "}\n";
  static const GLchar* fragment_shader =
      NK_SHADER_VERSION "precision mediump float;\n"
                        "uniform sampler2D Texture;\n"
                        "in vec2 Frag_UV;\n"
                        "in vec4 Frag_Color;\n"
                        "out vec4 Out_Color;\n"
                        "void main(){\n"
                        "   Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
                        "}\n";

  nk_buffer_init_default(&dev->cmds);
  dev->prog = gl->createProgram();
  dev->vert_shdr = gl->createShader(GL_VERTEX_SHADER);
  dev->frag_shdr = gl->createShader(GL_FRAGMENT_SHADER);
  gl->shaderSource(dev->vert_shdr, 1, &vertex_shader, 0);
  gl->shaderSource(dev->frag_shdr, 1, &fragment_shader, 0);
  gl->compileShader(dev->vert_shdr);
  gl->compileShader(dev->frag_shdr);
  gl->getShaderiv(dev->vert_shdr, GL_COMPILE_STATUS, &status);
  assert(status == GL_TRUE);
  gl->getShaderiv(dev->frag_shdr, GL_COMPILE_STATUS, &status);
  assert(status == GL_TRUE);
  gl->attachShader(dev->prog, dev->vert_shdr);
  gl->attachShader(dev->prog, dev->frag_shdr);
  gl->linkProgram(dev->prog);
  gl->getProgramiv(dev->prog, GL_LINK_STATUS, &status);
  assert(status == GL_TRUE);

  dev->uniform_tex = gl->getUniformLocation(dev->prog, "Texture");
  dev->uniform_proj = gl->getUniformLocation(dev->prog, "ProjMtx");
  dev->attrib_pos = gl->getAttribLocation(dev->prog, "Position");
  dev->attrib_uv = gl->getAttribLocation(dev->prog, "TexCoord");
  dev->attrib_col = gl->getAttribLocation(dev->prog, "Color");

  {
    /* buffer setup */
    GLsizei vs = sizeof(struct nk_glfw_vertex);
    size_t  vp = offsetof(struct nk_glfw_vertex, position);
    size_t  vt = offsetof(struct nk_glfw_vertex, uv);
    size_t  vc = offsetof(struct nk_glfw_vertex, col);

    gl->genBuffers(1, &dev->vbo);
    gl->genBuffers(1, &dev->ebo);
    gl->genVertexArrays(1, &dev->vao);

    gl->bindVertexArray(dev->vao);
    gl->bindBuffer(GL_ARRAY_BUFFER, dev->vbo);
    gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, dev->ebo);

    gl->enableVertexAttribArray((GLuint)dev->attrib_pos);
    gl->enableVertexAttribArray((GLuint)dev->attrib_uv);
    gl->enableVertexAttribArray((GLuint)dev->attrib_col);

    gl->vertexAttribPointer((GLuint)dev->attrib_pos, 2, GL_FLOAT, GL_FALSE, vs, (void*)vp);
    gl->vertexAttribPointer((GLuint)dev->attrib_uv, 2, GL_FLOAT, GL_FALSE, vs, (void*)vt);
    gl->vertexAttribPointer((GLuint)dev->attrib_col, 4, GL_UNSIGNED_BYTE, GL_TRUE, vs, (void*)vc);
  }

  gl->bindTexture(GL_TEXTURE_2D, 0);
  gl->bindBuffer(GL_ARRAY_BUFFER, 0);
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  gl->bindVertexArray(0);
}

static void device_upload_atlas(RonaGL* gl, NuklearState* dev, const void* image, int width,
                                int height) {
  gl->genTextures(1, &dev->font_tex);
  gl->bindTexture(GL_TEXTURE_2D, dev->font_tex);
  gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  gl->texImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, image);
}

static void device_shutdown(RonaGL* gl, NuklearState* dev) {
  gl->detachShader(dev->prog, dev->vert_shdr);
  gl->detachShader(dev->prog, dev->frag_shdr);
  gl->deleteShader(dev->vert_shdr);
  gl->deleteShader(dev->frag_shdr);
  gl->deleteProgram(dev->prog);
  gl->deleteTextures(1, &dev->font_tex);
  gl->deleteBuffers(1, &dev->vbo);
  gl->deleteBuffers(1, &dev->ebo);
  nk_buffer_free(&dev->cmds);
}

void nuklear_render(RonaGL* gl, NuklearState* dev, int width, int height,
                    enum nk_anti_aliasing AA) {
  struct nk_context* ctx = &dev->ctx;

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
  gl->useProgram(dev->prog);
  gl->uniform1i(dev->uniform_tex, 0);
  gl->uniformMatrix4fv(dev->uniform_proj, 1, GL_FALSE, &ortho[0][0]);
  {
    /* convert from command queue into draw list and draw to screen */
    const struct nk_draw_command* cmd;
    void *                        vertices, *elements;
    const nk_draw_index*          offset = NULL;

    /* allocate vertex and element buffer */
    gl->bindVertexArray(dev->vao);
    gl->bindBuffer(GL_ARRAY_BUFFER, dev->vbo);
    gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, dev->ebo);

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
      config.null = dev->null;
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
        nk_convert(ctx, &dev->cmds, &vbuf, &ebuf, &config);
      }
    }
    gl->unmapBuffer(GL_ARRAY_BUFFER);
    gl->unmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    /* iterate over and execute each draw command */
    nk_draw_foreach(cmd, ctx, &dev->cmds) {
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
    nk_buffer_clear(&dev->cmds);
  }

  /* default OpenGL state */
  gl->useProgram(0);
  gl->bindBuffer(GL_ARRAY_BUFFER, 0);
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  gl->bindVertexArray(0);
  gl->disable(GL_BLEND);
  gl->disable(GL_SCISSOR_TEST);
}
