#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#include "rona.h"
#include "platform.h"
#include "renderer.h"
#include "colour.h"

static GLuint CreateShaderType(RonaGl *gl, GLenum type, const char *source);
static GLuint CreateShaderProgram(RonaGl *gl, const char *name, const char *vertexSource, const char *fragmentSource);

/*
  This is a convoluted way of including text files into c source code

  The makefile calls the xxd utility which converts shaders in src/shader/ to
  c files in ./target. The #include followed by SHADER_AS_STRING macro then
  loads in those c files

  The benefit of all this is that hot reloading works on shaders
*/
#define SHADER_AS_STRING(NAME, SHADER)                                       \
  char *NAME = RONA_MALLOC(sizeof(char) * (src_shaders_##SHADER##_len + 1)); \
  memcpy(NAME, src_shaders_##SHADER, src_shaders_##SHADER##_len);            \
  NAME[src_shaders_##SHADER##_len] = 0;

void renderer_lib_load(GameState *game_state) {
  RonaGl *gl = &(game_state->gl);

  gl->genVertexArrays(1, &game_state->vertex_array_id);
  gl->bindVertexArray(game_state->vertex_array_id);

  #include "../target/shader.vs.c"
  SHADER_AS_STRING(vertexSource, shader_vs);

  #include "../target/shader.fs.c"
  SHADER_AS_STRING(fragmentSource, shader_fs);

  GLuint shaderProgram = CreateShaderProgram(&(game_state->gl), "Test", vertexSource, fragmentSource);

  RONA_FREE(vertexSource);
  RONA_FREE(fragmentSource);

  f32 vertices[] = {
    0.0f, 0.5f,
    -0.5f, -0.5f,
    0.5f, -0.5f
  };
  GLuint buffer;
  gl->genBuffers(1, &buffer);
  gl->bindBuffer(GL_ARRAY_BUFFER, buffer);
  gl->bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  gl->bindBuffer(GL_ARRAY_BUFFER, 0);

  gl->useProgram(shaderProgram);

  gl->bindBuffer(GL_ARRAY_BUFFER, buffer);
  gl->enableVertexAttribArray(0);
  gl->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);


  colour bg;
  colour_from(&bg, RGB, HSLuv, 250.0f, 90.0f, 60.0f, 1.0f);

  colour fg;
  colour_from(&fg, RGB, HSLuv, 120.0f, 90.0f, 60.0f, 1.0f);

  RONA_INFO("fg is %.2f, %.2f, %.2f, %.2f\n", fg.element[0], fg.element[1], fg.element[2], fg.element[3]);

  gl->clearColor(bg.element[0], bg.element[1], bg.element[2], bg.element[3]);
}

void renderer_lib_unload(GameState *game_state) {
  RonaGl *gl = &(game_state->gl);

  gl->disableVertexAttribArray(0);
  gl->bindBuffer(GL_ARRAY_BUFFER, 0);

  gl->bindVertexArray(0);
  gl->deleteVertexArrays(1, &game_state->vertex_array_id);
}

void renderer_step(GameState *game_state) {
  RonaGl *gl = &(game_state->gl);

  gl->viewport(0, 0, game_state->window_width, game_state->window_height);
  gl->clear(GL_COLOR_BUFFER_BIT);
  gl->drawArrays(GL_TRIANGLES, 0, 3);
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

static GLuint CreateShaderProgram(RonaGl *gl, const char *name, const char *vertexSource, const char *fragmentSource) {
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
    RONA_ERROR("Failed linking '%s' shader!\n", name);
    RONA_ERROR("%s\n", info);
  }

  gl->deleteShader(fragmentShader);
  gl->deleteShader(vertexShader);

  return(programId);
}
