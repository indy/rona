#ifndef RENDERER_H
#define RENDERER_H

void renderer_startup(RonaGl *gl);
void renderer_shutdown(RonaGl *gl);

void renderer_lib_load(RonaGl *gl);
void renderer_lib_unload(RonaGl *gl);

void renderer_render(RonaGl *gl, Level *level, i32 window_width, i32 window_height);
/*
  This is a convoluted way of including text files into c source code

  The makefile calls the xxd utility which converts shaders in src/shader/ to
  c files in ./target. The #include followed by SHADER_AS_STRING macro then
  loads in those c files

  The benefit of all this is that hot reloading works on shaders
*/
#define SHADER_AS_STRING(ARENA, NAME, SHADER)                            \
  char *NAME = (char *)ARENA_ALLOC(ARENA, (src_shaders_##SHADER##_len + 1)); \
  memcpy(NAME, src_shaders_##SHADER, src_shaders_##SHADER##_len);            \
  NAME[src_shaders_##SHADER##_len] = 0;


GLuint create_shader_program(RonaGl *gl, const char *vertexSource, const char *fragmentSource);

#endif /* RENDERER_H */
