#include "string.h"
#include "stdio.h"

#include "rona.h"
#include "platform.h"
#include "renderer.h"
#include "colour.h"
#include "memory_arena.h"

static GLuint CreateShaderType(RonaGl *gl, GLenum type, const char *source);
static GLuint CreateShaderProgram(RonaGl *gl, const char *vertexSource, const char *fragmentSource);

void renderer_step(GameState *game_state) {
  RonaGl *gl = &(game_state->gl);

  gl->viewport(0, 0, game_state->window_width, game_state->window_height);
  gl->clear(GL_COLOR_BUFFER_BIT);

  gl->bindVertexArray(game_state->vao);
  gl->drawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

/*
  This is a convoluted way of including text files into c source code

  The makefile calls the xxd utility which converts shaders in src/shader/ to
  c files in ./target. The #include followed by SHADER_AS_STRING macro then
  loads in those c files

  The benefit of all this is that hot reloading works on shaders
*/
#define SHADER_AS_STRING(ARENA, NAME, SHADER)                            \
  char *NAME = (char *)memory_arena_push(ARENA, (src_shaders_##SHADER##_len + 1)); \
  memcpy(NAME, src_shaders_##SHADER, src_shaders_##SHADER##_len);            \
  NAME[src_shaders_##SHADER##_len] = 0;

void renderer_lib_load(GameState *game_state) {
  RonaGl *gl = &(game_state->gl);

  gl->genVertexArrays(1, &game_state->vao); // Vertex Array Object
  gl->bindVertexArray(game_state->vao);

  #include "../target/shader.vert.c"
  SHADER_AS_STRING(&game_state->storage_transient, vertexSource, shader_vert);

  #include "../target/shader.frag.c"
  SHADER_AS_STRING(&game_state->storage_transient, fragmentSource, shader_frag);

  GLuint shaderProgram = CreateShaderProgram(&(game_state->gl), vertexSource, fragmentSource);

  f32 vertices[] = {
    -0.5f,  0.5f,
    -0.5f, -0.5f,
     0.5f, -0.5f,
     0.5f,  0.5f
  };
  u32 indices[] = {
    0, 1, 2,
    0, 2, 3
  };

  // the type of a Vertex Buffer Object is GL_ARRAY_BUFFER
  //
  GLuint vbo;
  gl->genBuffers(1, &vbo);
  gl->bindBuffer(GL_ARRAY_BUFFER, vbo);
  gl->bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // the data is set only once and used many times.
  gl->bindBuffer(GL_ARRAY_BUFFER, 0);

  GLuint ebo;
  gl->genBuffers(1, &ebo);
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  gl->bufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  gl->useProgram(shaderProgram);

  gl->bindBuffer(GL_ARRAY_BUFFER, vbo);
  gl->enableVertexAttribArray(0);
  gl->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);

  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  gl->bindVertexArray(0);

  Colour bg;
  colour_from(&bg, ColourFormat_sRGB, ColourFormat_HSLuv, 250.0f, 90.0f, 60.0f, 1.0f);

  gl->clearColor(bg.element[0], bg.element[1], bg.element[2], bg.element[3]);
}

void renderer_lib_unload(GameState *game_state) {
  RonaGl *gl = &(game_state->gl);

  gl->disableVertexAttribArray(0);
  gl->bindBuffer(GL_ARRAY_BUFFER, 0);

  gl->bindVertexArray(0);
  gl->deleteVertexArrays(1, &game_state->vao);
}

void renderer_startup(GameState *game_state) {
  RonaGl *gl = &(game_state->gl);

  const char *version = (const char *)game_state->gl.getString(GL_VERSION);
  const char *vendor = (const char *)game_state->gl.getString(GL_VENDOR);
  const char *renderer = (const char *)game_state->gl.getString(GL_RENDERER);
  RONA_INFO("OpenGL version: %s\n", version);
  RONA_INFO("OpenGL vendor: %s\n", vendor);
  RONA_INFO("OpenGL renderer: %s\n", renderer);

  const char *glslVersion = (const char *)game_state->gl.getString(GL_SHADING_LANGUAGE_VERSION);
  RONA_INFO("OpenGL GLSL Version %s:\n", glslVersion);

  int profileMask;
  int contextFlags;
  gl->getIntegerv(GL_CONTEXT_PROFILE_MASK, &profileMask);
  gl->getIntegerv(GL_CONTEXT_FLAGS, &contextFlags);
  RONA_INFO("OpenGL supported profiles:\n");
  RONA_INFO("\tCore: %s\n", ((profileMask & GL_CONTEXT_CORE_PROFILE_BIT) ? "yes" : "no"));
  RONA_INFO("\tForward: %s\n", ((contextFlags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT) ? "yes" : "no"));

  RONA_OUT("Running modern opengl\n");
}

void renderer_shutdown(GameState *game_state) {
}

static GLuint CreateShaderType(RonaGl *gl, GLenum type, const char *source) {
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
  if(!compileResult) {
    GLint infoLen;
    gl->getShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLen);
    // RONA_ASSERT(infoLen <= fplArrayCount(info));
    gl->getShaderInfoLog(shaderId, infoLen, &infoLen, info);
    RONA_ERROR("Failed compiling %s shader!\n", (type == GL_VERTEX_SHADER ? "vertex" : "fragment"));
    RONA_ERROR("%s\n", info);
  }

  return(shaderId);
}

static GLuint CreateShaderProgram(RonaGl *gl, const char *vertexSource, const char *fragmentSource) {
  GLuint programId = gl->createProgram();

  GLuint vertexShader = CreateShaderType(gl, GL_VERTEX_SHADER, vertexSource);
  GLuint fragmentShader = CreateShaderType(gl, GL_FRAGMENT_SHADER, fragmentSource);

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
  if(!linkResult) {
    GLint infoLen;
    gl->getProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLen);
    // RONA_ASSERT(infoLen <= fplArrayCount(info));
    gl->getProgramInfoLog(programId, infoLen, &infoLen, info);
    RONA_ERROR("%s\n", info);
  }

  gl->deleteShader(fragmentShader);
  gl->deleteShader(vertexShader);

  return(programId);
}
