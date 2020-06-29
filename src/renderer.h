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

#ifndef RENDERER_H
#define RENDERER_H

bool renderer_startup(RonaGl *gl, RenderStruct *render_struct);
void renderer_shutdown(RonaGl *gl);

void renderer_lib_load(RonaGl *gl, MemoryArena *transient, RenderStruct *render_struct);
void renderer_lib_unload(RonaGl *gl);

void renderer_render(RonaGl *gl, Level *level, RenderStruct *render_struct, Mesh *screen);
/*
  This is a convoluted way of including text files into c source code

  The makefile calls the xxd utility which converts shaders in src/shader/ to
  c files in ./target. The #include followed by SHADER_AS_STRING macro then
  loads in those c files

  The benefit of all this is that hot reloading works on shaders
*/
#define SHADER_AS_STRING(ARENA, NAME, SHADER)                                                      \
  char *NAME = (char *)ARENA_ALLOC(ARENA, (src_shaders_##SHADER##_len + 1));                       \
  memcpy(NAME, src_shaders_##SHADER, src_shaders_##SHADER##_len);                                  \
  NAME[src_shaders_##SHADER##_len] = 0;

GLuint create_shader_program(RonaGl *gl, const char *vertexSource, const char *fragmentSource);

#endif /* RENDERER_H */
