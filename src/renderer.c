#include "string.h"
#include "stdio.h"

#include "rona.h"
#include "platform.h"
#include "game_state.h"
#include "renderer.h"
#include "colour.h"
#include "memory_arena.h"
#include "rona_math.h"
#include "mesh_hero.h"

void renderer_render(RonaGl *gl, Level *level, i32 window_width, i32 window_height) {
  gl->viewport(0, 0, window_width, window_height);
  gl->clear(GL_COLOR_BUFFER_BIT);

  f32 aspect_ratio = (f32)window_width / (f32)window_height;
  f32 half_height = 5.0;
  f32 half_width = half_height * aspect_ratio;

  Mat4 proj_matrix;
  mat4_ortho(&proj_matrix, -half_width, half_width, -half_height, half_height, 10.0f, -10.0f);

  GLuint current_shader = 0;
  for(i32 i=0;i<level->max_num_entities;i++) {
    Entity *entity = &(level->entities[i]);
    if (!entity->exists) {
      break;                    // no more valid entities
    }
    Mesh *mesh = entity->mesh;
    if (current_shader != mesh->shader_program) {
      gl->useProgram(mesh->shader_program);
    }
    gl->uniform4f(mesh->uniform_colour, entity->colour.r, entity->colour.g, entity->colour.b, entity->colour.a);

    Mat4 transform_matrix;
    mat4_identity(&transform_matrix);
    transform_matrix.m14 = entity->world_pos.x;
    transform_matrix.m24 = entity->world_pos.y;

    Mat4 res_matrix;
    mat4_multiply(&res_matrix, &proj_matrix, &transform_matrix);

    gl->uniformMatrix4fv(mesh->uniform_proj_matrix, 1, false, (GLfloat *)&(res_matrix.v));

    gl->bindVertexArray(mesh->vao);
    gl->drawElements(GL_TRIANGLES, mesh->num_elements, GL_UNSIGNED_INT, 0);
  }
}

void renderer_lib_load(RonaGl *gl) {
  Colour bg;
  colour_from(&bg, ColourFormat_sRGB, ColourFormat_HSLuv, 250.0f, 90.0f, 60.0f, 1.0f);

  gl->clearColor(bg.element[0], bg.element[1], bg.element[2], bg.element[3]);
}

void renderer_lib_unload(RonaGl *gl) {
  gl->disableVertexAttribArray(0);
  gl->bindBuffer(GL_ARRAY_BUFFER, 0);

  gl->bindVertexArray(0);
}

void renderer_startup(RonaGl *gl) {
  const char *version = (const char *)gl->getString(GL_VERSION);
  const char *vendor = (const char *)gl->getString(GL_VENDOR);
  const char *renderer = (const char *)gl->getString(GL_RENDERER);
  RONA_INFO("OpenGL version: %s\n", version);
  RONA_INFO("OpenGL vendor: %s\n", vendor);
  RONA_INFO("OpenGL renderer: %s\n", renderer);

  const char *glslVersion = (const char *)gl->getString(GL_SHADING_LANGUAGE_VERSION);
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

void renderer_shutdown(RonaGl *gl) {
}


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
