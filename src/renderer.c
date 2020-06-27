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

GLuint create_framebuffer(RonaGl *gl);
GLuint create_texture(RonaGl *gl, u32 width, u32 height);
GLuint create_depth_texture(RonaGl *gl, u32 width, u32 height);
void delete_texture(RonaGl *gl, GLuint texture_id);
void attach_textures_to_framebuffer(RonaGl *gl, GLuint framebuffer_id, GLuint texture_id,
                                    GLuint depth_texture_id);
bool is_framebuffer_ok(RonaGl *gl);
void update_viewport(RonaGl *gl, u32 viewport_width, u32 viewport_height);
void bind_framebuffer(RonaGl *gl, GLuint framebuffer_id, u32 viewport_width, u32 viewport_height);

u32 g_tempo_texture;

void renderer_render(RonaGl *gl, Level *level, RenderStruct *render_struct, Mesh *screen) {
  bind_framebuffer(gl, render_struct->framebuffer_id, render_struct->render_texture_width,
                   render_struct->render_texture_height);

  gl->clearColor(0.0f, 0.0f, 0.1f, 0.0f);
  gl->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gl->disable(GL_DEPTH_TEST);

  gl->enable(GL_BLEND);
  gl->blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  f32 render_texture_width = (f32)render_struct->render_texture_width;
  f32 render_texture_height = (f32)render_struct->render_texture_height;
  // this is always a 640x360 texture render target
  f32 aspect_ratio = render_texture_width / render_texture_height;
  f32 height;
  f32 width;

  // f32 level_aspect_ratio = (f32)level->width / (f32)level-height;
  if (aspect_ratio > 1.0) {
    // the window is wider than it is tall
    //
    height = (f32)(level->height + 1);
    width = height * aspect_ratio;
  } else {
    // the window is taller than it is wide
    width = (f32)(level->width + 1);
    height = width / aspect_ratio;
  }

  GLuint current_shader = 0;

  // render level's floor
#if 0
  Mesh *mesh = level->mesh_floor;
  if (current_shader != mesh->shader_program) {
    gl->useProgram(mesh->shader_program);

    Mat4 proj_matrix = mat4_ortho(-1.0, width, -1.0, height, 10.0f, -10.0f);
    gl->uniformMatrix4fv(mesh->uniform_proj_matrix, 1, false, (GLfloat *)&(proj_matrix.v));
  }

  Colour ground_colour;
  colour_from(&ground_colour, ColourFormat_RGB, ColourFormat_HSLuv, 60.0f, 80.0f, 70.0f, 1.0f);
  gl->uniform4f(mesh->uniform_colour, ground_colour.element[0], ground_colour.element[1],
                ground_colour.element[2], ground_colour.element[3]);

  f32 world_pos_x = 0.0f;
  f32 world_pos_y = 0.0f;
  gl->uniform3f(mesh->uniform_pos, world_pos_x, world_pos_y, 2.0f);

  gl->bindVertexArray(mesh->vao);
  gl->drawElements(GL_TRIANGLES, mesh->num_elements, GL_UNSIGNED_INT, 0);
#endif
  // render entities

  for (i32 i = 0; i < level->max_num_entities; i++) {
    Entity *entity = &(level->entities[i]);
    if (!entity->exists) {
      break;
    }
    Mesh *mesh = entity->mesh;
    if (current_shader != mesh->shader_program) {
      gl->useProgram(mesh->shader_program);

      Mat4 proj_matrix = mat4_ortho(-1.0, width, -1.0, height, 10.0f, -10.0f);
      gl->uniformMatrix4fv(mesh->uniform_proj_matrix, 1, false, (GLfloat *)&(proj_matrix.v));
    }


    gl->uniform1i(mesh->uniform_texture, 1);
    gl->activeTexture(GL_TEXTURE1);
    gl->bindTexture(GL_TEXTURE_2D, g_tempo_texture);


    gl->uniform4f(mesh->uniform_colour, entity->colour.r, entity->colour.g, entity->colour.b,
                  entity->colour.a);
    gl->uniform3f(mesh->uniform_pos, entity->world_pos.x, entity->world_pos.y, entity->world_pos.z);

    gl->bindVertexArray(mesh->vao);
    gl->drawElements(GL_TRIANGLES, mesh->num_elements, GL_UNSIGNED_INT, 0);
  }

  // render texture to screen

  bind_framebuffer(gl, 0, render_struct->window_width, render_struct->window_height);

  gl->clearColor(0.0f, 0.0f, 0.0f, 0.0f);
  gl->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gl->useProgram(screen->shader_program);

  f32 window_aspect_ratio = (f32)render_struct->window_width / (f32)render_struct->window_height;

  if (window_aspect_ratio <= aspect_ratio) {
    // window is narrower than desired
    f32 v = (aspect_ratio / window_aspect_ratio) * render_texture_height;
    f32 v_pad = (v - render_texture_height) / 2.0f;
    Mat4 m = mat4_ortho(0.0f, render_texture_width, -v_pad, v - v_pad, 10.0f, -10.0f);
    gl->uniformMatrix4fv(screen->uniform_proj_matrix, 1, false, (GLfloat *)&(m.v));
  } else {
    // window is more elongated horizontally than desired
    f32 h = (window_aspect_ratio / aspect_ratio) * render_texture_width;
    f32 h_pad = (h - render_texture_width) / 2.0f;
    Mat4 m = mat4_ortho(-h_pad, h - h_pad, 0, render_texture_height, 10.0f, -10.0f);
    gl->uniformMatrix4fv(screen->uniform_proj_matrix, 1, false, (GLfloat *)&(m.v));
  }

  gl->uniform1i(screen->uniform_texture, 0);
  gl->activeTexture(GL_TEXTURE0);
  gl->bindTexture(GL_TEXTURE_2D, render_struct->render_texture_id);

  gl->bindVertexArray(screen->vao);
  gl->drawElements(GL_TRIANGLES, screen->num_elements, GL_UNSIGNED_INT, 0);
}

void renderer_lib_load(RonaGl *gl) {
  Colour bg;
  colour_from(&bg, ColourFormat_RGB, ColourFormat_HSLuv, 250.0f, 90.0f, 60.0f, 0.0f);

  gl->clearColor(bg.element[0], bg.element[1], bg.element[2], 0.0);
}

void renderer_lib_unload(RonaGl *gl) {
  gl->disableVertexAttribArray(0);
  gl->bindBuffer(GL_ARRAY_BUFFER, 0);

  gl->bindVertexArray(0);
}

void renderer_startup(RonaGl *gl, RenderStruct *render_struct) {
  const char *version = (const char *)gl->getString(GL_VERSION);
  const char *vendor = (const char *)gl->getString(GL_VENDOR);
  const char *renderer = (const char *)gl->getString(GL_RENDERER);
  RONA_INFO("OpenGL version: %s\n", version);
  RONA_INFO("OpenGL vendor: %s\n", vendor);
  RONA_INFO("OpenGL renderer: %s\n", renderer);

  const char *glslVersion = (const char *)gl->getString(GL_SHADING_LANGUAGE_VERSION);
  RONA_INFO("OpenGL GLSL Version %s:\n", glslVersion);

  // int profileMask;
  // int contextFlags;
  // gl->getIntegerv(GL_CONTEXT_PROFILE_MASK, &profileMask);
  // gl->getIntegerv(GL_CONTEXT_FLAGS, &contextFlags);
  // RONA_INFO("OpenGL supported profiles:\n");
  // RONA_INFO("\tCore: %s\n", ((profileMask & GL_CONTEXT_CORE_PROFILE_BIT) ? "yes" : "no"));
  // RONA_INFO("\tForward: %s\n", ((contextFlags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT) ? "yes" :
  // "no"));


  // load textures
  gl->genTextures(1, &g_tempo_texture);
  int tex_width, tex_height, num_channels;
  unsigned char *data = stbi_load("assets/test3.png", &tex_width, &tex_height, &num_channels, 0);
  RONA_INFO("width %d height %d channels %d\n", tex_width, tex_height, num_channels);
  for (int i = 0; i < 40; i++) {
    RONA_LOG("%d: %d\n", i, data[i]);
  }

  if (data) {
    gl->activeTexture(GL_TEXTURE1);
    gl->bindTexture(GL_TEXTURE_2D, g_tempo_texture);

    gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    gl->texImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //gl->generateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
  }

  i32 width = 640 * 2;
  i32 height = 360 * 2;;

  // setup render texture
  render_struct->render_texture_width = width;
  render_struct->render_texture_height = height;
  render_struct->render_texture_id = create_texture(gl, width, height);
  render_struct->depth_texture_id = create_depth_texture(gl, width, height);
  render_struct->framebuffer_id = create_framebuffer(gl);

  attach_textures_to_framebuffer(gl, render_struct->framebuffer_id,
                                 render_struct->render_texture_id, render_struct->depth_texture_id);
  if (!is_framebuffer_ok(gl)) {
    RONA_ERROR("%d, Framebuffer is not ok\n", 1);
  }
  gl->bindFramebuffer(GL_FRAMEBUFFER, render_struct->framebuffer_id);
  gl->viewport(0, 0, width, height);

  RONA_OUT("Running modern opengl\n");
}

void renderer_shutdown(RonaGl *gl) {}

GLuint create_shader_type(RonaGl *gl, GLenum type, const char *source) {
  GLuint shaderId = gl->createShader(type);

  gl->shaderSource(shaderId, 1, &source, NULL);
  gl->compileShader(shaderId);

  char info[1024 * 10];
  int i = 0;
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

GLuint create_shader_program(RonaGl *gl, const char *vertexSource, const char *fragmentSource) {
  GLuint programId = gl->createProgram();

  GLuint vertexShader = create_shader_type(gl, GL_VERTEX_SHADER, vertexSource);
  GLuint fragmentShader = create_shader_type(gl, GL_FRAGMENT_SHADER, fragmentSource);

  gl->attachShader(programId, vertexShader);
  gl->attachShader(programId, fragmentShader);
  gl->linkProgram(programId);
  gl->validateProgram(programId);

  char info[1024 * 10];
  int i = 0;
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

  gl->deleteShader(fragmentShader);
  gl->deleteShader(vertexShader);

  return (programId);
}

GLuint create_framebuffer(RonaGl *gl) {
  GLuint framebuffer;
  gl->genFramebuffers(1, &framebuffer);

  return framebuffer;
}

GLuint create_texture(RonaGl *gl, u32 width, u32 height) {
  GLuint texture_id;

  gl->genTextures(1, &texture_id);
  // bind so that all future texture ops happen to this texture
  gl->bindTexture(GL_TEXTURE_2D, texture_id);
  gl->texImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, NULL);

  gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  gl->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  gl->bindTexture(GL_TEXTURE_2D, 0);

  return texture_id;
}

GLuint create_depth_texture(RonaGl *gl, u32 width, u32 height) {
  GLuint texture_id;

  gl->genTextures(1, &texture_id);
  // bind so that all future texture ops happen to this texture
  gl->bindTexture(GL_TEXTURE_2D, texture_id);
  gl->texImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, (GLsizei)width, (GLsizei)height, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

  gl->bindTexture(GL_TEXTURE_2D, 0);

  return texture_id;
}

void delete_texture(RonaGl *gl, GLuint texture_id) { gl->deleteTextures(1, &texture_id); }

void attach_textures_to_framebuffer(RonaGl *gl, GLuint framebuffer_id, GLuint texture_id,
                                    GLuint depth_texture_id) {
  gl->bindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
  gl->framebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);
  gl->framebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture_id, 0);
}

bool is_framebuffer_ok(RonaGl *gl) {
  if (gl->checkFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    RONA_ERROR("Framebuffer is not complete\n");
    return false;
  }
  return true;
}

void update_viewport(RonaGl *gl, u32 viewport_width, u32 viewport_height) {
  gl->viewport(0, 0, viewport_width, viewport_height);
}

void bind_framebuffer(RonaGl *gl, GLuint framebuffer_id, u32 viewport_width, u32 viewport_height) {
  gl->bindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
  update_viewport(gl, viewport_width, viewport_height);
}
