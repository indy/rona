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

void delete_texture(RonaGL* gl, GLuint texture_id);
void update_viewport(RonaGL* gl, u32 viewport_width, u32 viewport_height);
void bind_framebuffer(RonaGL* gl, GLuint framebuffer_id, u32 viewport_width, u32 viewport_height);

void renderer_render(GameState* game_state) {
  RonaGL*       gl            = game_state->gl;
  Level*        level         = game_state->level;
  RenderStruct* render_struct = &game_state->render_struct;
  Graphic*      screen        = &(game_state->screen_graphic);

  // render the scene onto the stage texture
  //
  bind_framebuffer(gl, render_struct->framebuffer_id, render_struct->stage_width,
                   render_struct->stage_height);

  gl->clearColor(0.1f, 0.1f, 0.1f, 1.0f);
  gl->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gl->disable(GL_DEPTH_TEST);

  gl->enable(GL_BLEND);
  gl->blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  gl->activeTexture(GL_TEXTURE1);
  gl->bindTexture(GL_TEXTURE_2D, render_struct->tileset_texture_id);

  f32 stage_width  = (f32)render_struct->stage_width;
  f32 stage_height = (f32)render_struct->stage_height;

  // use the RenderStruct's shader_tile for all tile based entities
  //
  gl->useProgram(render_struct->shader_tile.program);
  gl->uniform1i(render_struct->shader_tile.uniform_texture, 1);

  Mat4 proj_matrix = mat4_ortho(0.0, stage_width, 0.0f, stage_height, 10.0f, -10.0f);
  gl->uniformMatrix4fv(render_struct->shader_tile.uniform_proj_matrix, 1, false, (GLfloat*)&(proj_matrix.v));

  // render level's chunks
  //
  {
    Graphic* graphic = &(level->chunks_graphic);
    gl->uniform3f(render_struct->shader_tile.uniform_pos, (f32)(-level->viewport.pos.x * TILE_WIDTH),
                  (f32)(-level->viewport.pos.y * TILE_HEIGHT), 2.0f);
    gl->bindVertexArray(graphic->vao);
    gl->drawElements(GL_TRIANGLES, graphic->num_elements, GL_UNSIGNED_INT, 0);
  }

  // render entities
  //
  {
    Graphic* graphic = &(level->entities_graphic);
    gl->uniform3f(render_struct->shader_tile.uniform_pos, (f32)(-level->viewport.pos.x * TILE_WIDTH),
                  (f32)(-level->viewport.pos.y * TILE_HEIGHT), 2.0f);
    gl->bindVertexArray(graphic->vao);
    gl->drawElements(GL_TRIANGLES, graphic->num_elements, GL_UNSIGNED_INT, 0);
  }

  // render text
  //
  // gl->activeTexture(GL_TEXTURE2);
  // gl->bindTexture(GL_TEXTURE_2D, render_struct->font_texture_id);
  // gl->uniform1i(render_struct->shader_tile.uniform_texture, 2);

  gl->uniform3f(render_struct->shader_tile.uniform_pos, 0.0f, 0.0f, 0.0f);
  gl->bindVertexArray(render_struct->text_vao);
  gl->drawElements(GL_TRIANGLES, render_struct->num_characters * TILED_QUAD_NUM_INDICES, GL_UNSIGNED_INT, 0);

  // --------------------------------------------------------------------------------

  // if RONA_EDITOR isn't defined just render as if GameMode_Play was selected

#ifdef RONA_EDITOR
  if (game_state->mode == GameMode_Play) {
#endif

    // render stage as large as possible onto screen
    //
    bind_framebuffer(gl, 0, render_struct->window_width, render_struct->window_height);
    gl->clearColor(0.0f, 0.0f, 0.0f, 0.0f);
    gl->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gl->useProgram(render_struct->shader_screen.program);

    f32 aspect_ratio        = stage_width / stage_height;
    f32 window_aspect_ratio = (f32)render_struct->window_width / (f32)render_struct->window_height;

    if (window_aspect_ratio <= aspect_ratio) {
      // window is narrower than desired
      f32  v     = (aspect_ratio / window_aspect_ratio) * stage_height;
      f32  v_pad = (v - stage_height) / 2.0f;
      Mat4 m     = mat4_ortho(0.0f, stage_width, v - v_pad, -v_pad, 10.0f, -10.0f);
      gl->uniformMatrix4fv(render_struct->shader_screen.uniform_proj_matrix, 1, false, (GLfloat*)&(m.v));
    } else {
      // window is more elongated horizontally than desired
      f32  h     = (window_aspect_ratio / aspect_ratio) * stage_width;
      f32  h_pad = (h - stage_width) / 2.0f;
      Mat4 m     = mat4_ortho(-h_pad, h - h_pad, stage_height, 0.0f, 10.0f, -10.0f);
      gl->uniformMatrix4fv(render_struct->shader_screen.uniform_proj_matrix, 1, false, (GLfloat*)&(m.v));
    }

    gl->uniform1i(render_struct->shader_screen.uniform_texture, 0);
    gl->activeTexture(GL_TEXTURE0);

    gl->bindTexture(GL_TEXTURE_2D, render_struct->stage_texture_id);

    gl->bindVertexArray(screen->vao);
    gl->drawElements(GL_TRIANGLES, screen->num_elements, GL_UNSIGNED_INT, 0);

#ifdef RONA_EDITOR
  } else {
    // GameMode_Edit

    // render the stage onto another texture using the shader_screen
    // this will perform sRGB colour conversion (and any other post processing effects)
    //
    bind_framebuffer(gl, editor_state.framebuffer_id, render_struct->stage_width,
                     render_struct->stage_height);
    gl->clearColor(0.0f, 0.0f, 0.0f, 0.0f);
    gl->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gl->useProgram(render_struct->shader_screen.program);

    Mat4 m = mat4_ortho(0.0f, stage_width, 0.0f, stage_height, 10.0f, -10.0f);
    gl->uniformMatrix4fv(render_struct->shader_screen.uniform_proj_matrix, 1, false, (GLfloat*)&(m.v));

    gl->uniform1i(render_struct->shader_screen.uniform_texture, 0);
    gl->activeTexture(GL_TEXTURE0);

    gl->bindTexture(GL_TEXTURE_2D, render_struct->stage_texture_id);

    gl->bindVertexArray(screen->vao);
    gl->drawElements(GL_TRIANGLES, screen->num_elements, GL_UNSIGNED_INT, 0);

    // render the edit ui onto the screen

    bind_framebuffer(gl, 0, render_struct->window_width, render_struct->window_height);
    gl->clearColor(0.0f, 0.0f, 0.0f, 0.0f);
    gl->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int width  = render_struct->window_width;
    int height = render_struct->window_height;

    editor_render(gl, &editor_state, width, height, &(render_struct->shader_editor));
  }
#endif
}

void renderer_lib_load(RonaGL* gl, BumpAllocator* transient, RenderStruct* render_struct) {
  Colour bg;
  colour_from(&bg, ColourFormat_RGB, ColourFormat_HSLuv, 250.0f, 90.0f, 60.0f, 0.0f);

#include "../target/tile.vert.c"
  SHADER_AS_STRING(transient, tileVertexSource, tile_vert);

#include "../target/tile.frag.c"
  SHADER_AS_STRING(transient, tileFragmentSource, tile_frag);

  ShaderTile* shader_tile          = &(render_struct->shader_tile);
  shader_tile->program             = create_shader_program(gl, tileVertexSource, tileFragmentSource);
  shader_tile->uniform_texture     = gl->getUniformLocation(shader_tile->program, "tilesheet");
  shader_tile->uniform_colour_fg   = gl->getUniformLocation(shader_tile->program, "colour_fg");
  shader_tile->uniform_colour_bg   = gl->getUniformLocation(shader_tile->program, "colour_bg");
  shader_tile->uniform_proj_matrix = gl->getUniformLocation(shader_tile->program, "proj_matrix");
  shader_tile->uniform_pos         = gl->getUniformLocation(shader_tile->program, "pos");

#include "../target/screen.vert.c"
  SHADER_AS_STRING(transient, screenVertexSource, screen_vert);

#include "../target/screen.frag.c"
  SHADER_AS_STRING(transient, screenFragmentSource, screen_frag);

  ShaderScreen* shader_screen        = &(render_struct->shader_screen);
  shader_screen->program             = create_shader_program(gl, screenVertexSource, screenFragmentSource);
  shader_screen->uniform_texture     = gl->getUniformLocation(shader_screen->program, "screen_texture");
  shader_screen->uniform_proj_matrix = gl->getUniformLocation(shader_screen->program, "proj_matrix");

  // --------------------------------------------------------------------------------

#ifdef RONA_EDITOR

#include "../target/editor.vert.c"
  SHADER_AS_STRING(transient, editorVertexSource, editor_vert);

#include "../target/editor.frag.c"
  SHADER_AS_STRING(transient, editorFragmentSource, editor_frag);

  ShaderEditor* shader_editor = &(render_struct->shader_editor);
  shader_editor->program      = create_shader_program(gl, editorVertexSource, editorFragmentSource);

  shader_editor->uniform_texture     = gl->getUniformLocation(shader_editor->program, "nuklear_texture");
  shader_editor->uniform_proj_matrix = gl->getUniformLocation(shader_editor->program, "proj_matrix");
#endif
  // --------------------------------------------------------------------------------

  gl->clearColor(bg.element[0], bg.element[1], bg.element[2], 0.0);
}

void renderer_lib_unload(RonaGL* gl, RenderStruct* render_struct) {
  // the earlier calls to deleteShader mean that deleteProgram  will also delete the shaders

  ShaderTile* shader_tile = &(render_struct->shader_tile);
  gl->deleteProgram(shader_tile->program);

  ShaderScreen* shader_screen = &(render_struct->shader_screen);
  gl->deleteProgram(shader_screen->program);

#ifdef RONA_EDITOR
  ShaderEditor* shader_editor = &(render_struct->shader_editor);
  gl->deleteProgram(shader_editor->program);
#endif

  gl->disableVertexAttribArray(0);
  gl->bindBuffer(GL_ARRAY_BUFFER, 0);

  gl->bindVertexArray(0);
}

bool renderer_startup(RonaGL* gl, RenderStruct* render_struct, BumpAllocator* permanent) {
  const char* version  = (const char*)gl->getString(GL_VERSION);
  const char* vendor   = (const char*)gl->getString(GL_VENDOR);
  const char* renderer = (const char*)gl->getString(GL_RENDERER);
  RONA_INFO("OpenGL version: %s\n", version);
  RONA_INFO("OpenGL vendor: %s\n", vendor);
  RONA_INFO("OpenGL renderer: %s\n", renderer);

  const char* glslVersion = (const char*)gl->getString(GL_SHADING_LANGUAGE_VERSION);
  RONA_INFO("OpenGL GLSL Version %s:\n", glslVersion);

  // int profileMask;
  // int contextFlags;
  // gl->getIntegerv(GL_CONTEXT_PROFILE_MASK, &profileMask);
  // gl->getIntegerv(GL_CONTEXT_FLAGS, &contextFlags);
  // RONA_INFO("OpenGL supported profiles:\n");
  // RONA_INFO("\tCore: %s\n", ((profileMask & GL_CONTEXT_CORE_PROFILE_BIT) ? "yes" : "no"));
  // RONA_INFO("\tForward: %s\n", ((contextFlags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT) ? "yes" :
  // "no"));

  // load tileset texture
  gl->genTextures(1, &(render_struct->tileset_texture_id));
  int            tex_width, tex_height, num_channels;
  char*          tileset_filename = "assets/tileset.png";
  unsigned char* data             = stbi_load(tileset_filename, &tex_width, &tex_height, &num_channels, 0);

  if (data) {
    gl->activeTexture(GL_TEXTURE1);
    gl->bindTexture(GL_TEXTURE_2D, render_struct->tileset_texture_id);

    gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    gl->texImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    // gl->generateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    Tileset* tileset           = &(render_struct->tileset);
    tileset->image_dim.width   = tex_width;
    tileset->image_dim.height  = tex_height;
    tileset->sprite_dim.width  = 16;
    tileset->sprite_dim.height = 16;
    tileset_calc_uv_units(tileset);

  } else {
    RONA_ERROR("renderer.c failed to load %s\n", tileset_filename);
    return false;
  }

  render_struct->char_locations                 = sprite_build_char_locations(permanent);
  render_struct->sprite_info                    = sprite_build_sprite_info(permanent);
  render_struct->animated_character_sprite_info = sprite_build_animated_character_sprite_info(permanent);

#if 0
  // load font texture
  gl->genTextures(1, &(render_struct->font_texture_id));
  tileset_filename = "assets/RulerGold.png"; // note: this font isn't monospace
  data = stbi_load(tileset_filename, &tex_width, &tex_height, &num_channels, 0);

  if (data) {
    gl->activeTexture(GL_TEXTURE2);
    gl->bindTexture(GL_TEXTURE_2D, render_struct->font_texture_id);

    gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    gl->texImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    // gl->generateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    Tileset* tileset = &(render_struct->font_tileset);
    tileset->image_dim.width = tex_width;
    tileset->image_dim.height = tex_height;
    tileset->sprite_dim.width = 8;
    tileset->sprite_dim.height = 15;
    tileset_calc_uv_units(tileset);

    RONA_LOG("uv %.6f, %.6f\n", tileset->uv_unit.u, tileset->uv_unit.v);

  } else {
    RONA_ERROR("renderer.c failed to load %s\n", tileset_filename);
    return false;
  }
#endif

  // setup render texture
  render_struct->stage_width      = STAGE_WIDTH;
  render_struct->stage_height     = STAGE_HEIGHT;
  render_struct->stage_texture_id = create_texture(gl, STAGE_WIDTH, STAGE_HEIGHT);
  render_struct->depth_texture_id = create_depth_texture(gl, STAGE_WIDTH, STAGE_HEIGHT);
  render_struct->framebuffer_id   = create_framebuffer(gl);

  attach_textures_to_framebuffer(gl, render_struct->framebuffer_id, render_struct->stage_texture_id,
                                 render_struct->depth_texture_id);
  if (!is_framebuffer_ok(gl)) {
    RONA_ERROR("%d, Framebuffer is not ok\n", 1);
    return false;
  }
  gl->bindFramebuffer(GL_FRAMEBUFFER, render_struct->framebuffer_id);
  gl->viewport(0, 0, STAGE_WIDTH, STAGE_HEIGHT);

  // play with text rendering
  //
  // allocate some memory for text rendering
  render_struct->max_characters_per_frame = 5000;
  render_struct->text_vertices_mem_allocated =
      render_struct->max_characters_per_frame * TILED_QUAD_GEOMETRY_BYTES;
  render_struct->text_vertices = (f32*)BUMP_ALLOC(permanent, render_struct->text_vertices_mem_allocated);
  render_struct->text_indices_mem_allocated =
      render_struct->max_characters_per_frame * TILED_QUAD_INDICES_BYTES;
  render_struct->text_indices = (u32*)BUMP_ALLOC(permanent, render_struct->text_indices_mem_allocated);

  gl->genVertexArrays(1, &(render_struct->text_vao));
  gl->bindVertexArray(render_struct->text_vao);
  gl->genBuffers(1, &(render_struct->text_vbo));
  gl->bindBuffer(GL_ARRAY_BUFFER, render_struct->text_vbo);

  gl->enableVertexAttribArray(0);
  gl->enableVertexAttribArray(1);
  gl->enableVertexAttribArray(2);
  gl->enableVertexAttribArray(3);

  // positions
  gl->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (void*)(0 * sizeof(float)));
  // uv
  gl->vertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (void*)(2 * sizeof(float)));
  // fg colour
  gl->vertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (void*)(4 * sizeof(float)));
  // bg colour
  gl->vertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (void*)(8 * sizeof(float)));

  gl->genBuffers(1, &(render_struct->text_ebo));
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_struct->text_ebo);

  gl->bindVertexArray(0);

  RONA_LOG("Running modern opengl\n");

  return true;
}

void text_render_reset(RenderStruct* render_struct) {
  render_struct->num_characters = 0;
}

void text_paragraph(TextParams* text_params, char* text) {
  char* c = text;

  Vec2 pos         = text_params->pos;
  f32  basex       = pos.x;
  i32  width_count = 0;

  while (*c) {
    if (*c == '\n') {
      pos.y += TILE_CHAR_HEIGHT;
      width_count = 0;
    } else {
      pos.x = basex + ((f32)(width_count * TILE_CHAR_WIDTH));
      tileset_add_char(text_params->render_struct, *c, &pos, &text_params->fg, &text_params->bg);
      width_count++;
    }
    c++;
  }
}

void text_printf(TextParams* text_params, char* fmt, ...) {
#define MAX_TEXT_PRINTF_BUFFER_SIZE 4096
  BumpAllocator* bump                 = text_params->bump;
  char*          buffer               = (char*)bump_head(bump);
  u64            bump_space_available = bump->size - bump->used;
  va_list        va;

  va_start(va, fmt);
  stbsp_vsnprintf(buffer, MIN(bump_space_available, MAX_TEXT_PRINTF_BUFFER_SIZE), fmt, va);
  va_end(va);

  // stbsp_sprintf(buf, "mouse co-ords (%d, %d)", game_params->input->mouse_pos.x,
  // game_params->input->mouse_pos.y);
  text_paragraph(text_params, buffer);
}

void text_send_to_gpu(RenderStruct* render_struct, RonaGL* gl) {
  gl->bindVertexArray(render_struct->text_vao);

  gl->bindBuffer(GL_ARRAY_BUFFER, render_struct->text_vbo);
  gl->bufferData(GL_ARRAY_BUFFER, render_struct->num_characters * TILED_QUAD_GEOMETRY_BYTES,
                 render_struct->text_vertices, GL_DYNAMIC_DRAW);

  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_struct->text_ebo);
  gl->bufferData(GL_ELEMENT_ARRAY_BUFFER, render_struct->num_characters * TILED_QUAD_INDICES_BYTES,
                 render_struct->text_indices, GL_DYNAMIC_DRAW);

  gl->bindVertexArray(0);
}

void renderer_shutdown(RonaGL* gl) {
}

GLuint create_shader_type(RonaGL* gl, GLenum type, const char* source) {
  GLuint shaderId = gl->createShader(type);

  gl->shaderSource(shaderId, 1, &source, NULL);
  gl->compileShader(shaderId);

  char info[1024 * 10];
  int  i = 0;
  for (i = 0; i < 1024 * 10; i++) {
    info[i] = 0;
  }

  GLint compileResult;
  gl->getShaderiv(shaderId, GL_COMPILE_STATUS, &compileResult);
  if (!compileResult) {
    GLint infoLen;
    gl->getShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLen);
    // RONA_ASSERT(infoLen <= fplArrayCount(info));
    gl->getShaderInfoLog(shaderId, infoLen, &infoLen, info);
    RONA_ERROR("Failed compiling %s shader!\n", (type == GL_VERTEX_SHADER ? "vertex" : "fragment"));
    RONA_ERROR("%s\n", info);
  }

  return (shaderId);
}

GLuint create_shader_program(RonaGL* gl, const char* vertexSource, const char* fragmentSource) {
  GLuint programId = gl->createProgram();

  GLuint vertexShader   = create_shader_type(gl, GL_VERTEX_SHADER, vertexSource);
  GLuint fragmentShader = create_shader_type(gl, GL_FRAGMENT_SHADER, fragmentSource);

  gl->attachShader(programId, vertexShader);
  gl->attachShader(programId, fragmentShader);
  gl->linkProgram(programId);
  gl->validateProgram(programId);

  char info[1024 * 10];
  int  i = 0;
  for (i = 0; i < 1024 * 10; i++) {
    info[i] = 0;
  }

  GLint linkResult;
  gl->getProgramiv(programId, GL_LINK_STATUS, &linkResult);
  if (!linkResult) {
    GLint infoLen;
    gl->getProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLen);
    // RONA_ASSERT(infoLen <= fplArrayCount(info));
    gl->getProgramInfoLog(programId, infoLen, &infoLen, info);
    RONA_ERROR("%s\n", info);
  }

  // http://docs.gl/gl3/glDeleteShader
  // If a shader object to be deleted is attached to a program object, it will be flagged for
  // deletion
  gl->deleteShader(fragmentShader);
  gl->deleteShader(vertexShader);

  return (programId);
}

GLuint create_framebuffer(RonaGL* gl) {
  GLuint framebuffer;
  gl->genFramebuffers(1, &framebuffer);

  return framebuffer;
}

GLuint create_texture(RonaGL* gl, u32 width, u32 height) {
  GLuint texture_id;

  gl->genTextures(1, &texture_id);
  // bind so that all future texture ops happen to this texture
  gl->bindTexture(GL_TEXTURE_2D, texture_id);
  gl->texImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 NULL);

  gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  gl->bindTexture(GL_TEXTURE_2D, 0);

  return texture_id;
}

GLuint create_depth_texture(RonaGL* gl, u32 width, u32 height) {
  GLuint texture_id;

  gl->genTextures(1, &texture_id);
  // bind so that all future texture ops happen to this texture
  gl->bindTexture(GL_TEXTURE_2D, texture_id);
  gl->texImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, (GLsizei)width, (GLsizei)height, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

  gl->bindTexture(GL_TEXTURE_2D, 0);

  return texture_id;
}

void delete_texture(RonaGL* gl, GLuint texture_id) {
  gl->deleteTextures(1, &texture_id);
}

void attach_textures_to_framebuffer(RonaGL* gl, GLuint framebuffer_id, GLuint texture_id,
                                    GLuint depth_texture_id) {
  gl->bindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
  gl->framebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);
  gl->framebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture_id, 0);
}

bool is_framebuffer_ok(RonaGL* gl) {
  if (gl->checkFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    RONA_ERROR("Framebuffer is not complete\n");
    return false;
  }
  return true;
}

void update_viewport(RonaGL* gl, u32 viewport_width, u32 viewport_height) {
  gl->viewport(0, 0, viewport_width, viewport_height);
}

void bind_framebuffer(RonaGL* gl, GLuint framebuffer_id, u32 viewport_width, u32 viewport_height) {
  gl->bindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
  update_viewport(gl, viewport_width, viewport_height);
}
