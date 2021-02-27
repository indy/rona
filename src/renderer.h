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

#ifndef RENDERER_H
#define RENDERER_H

bool renderer_startup(RonaGL* gl, RenderStruct* render_struct, BumpAllocator* permanent);
void renderer_shutdown(RonaGL* gl);

void renderer_lib_load(RonaGL* gl, BumpAllocator* transient, RenderStruct* render_struct);
void renderer_lib_unload(RonaGL* gl, RenderStruct* render_struct);

void renderer_render(GameState* game_state);

GLuint create_texture(RonaGL* gl, u32 width, u32 height);
GLuint create_depth_texture(RonaGL* gl, u32 width, u32 height);
GLuint create_framebuffer(RonaGL* gl);
bool   is_framebuffer_ok(RonaGL* gl);
void   attach_textures_to_framebuffer(RonaGL* gl, GLuint framebuffer_id, GLuint texture_id,
                                      GLuint depth_texture_id);

/*
  This is a convoluted way of including text files into c source code

  The makefile calls the xxd utility which converts shaders in src/shader/ to
  c files in ./target. The #include followed by SHADER_AS_STRING macro then
  loads in those c files

  The benefit of all this is that hot reloading works on shaders
*/
#define SHADER_AS_STRING(BUMP, NAME, SHADER)                                                                 \
  char* NAME = (char*)BUMP_ALLOC(BUMP, (src_shaders_##SHADER##_len + 1));                                    \
  memcpy(NAME, src_shaders_##SHADER, src_shaders_##SHADER##_len);                                            \
  NAME[src_shaders_##SHADER##_len] = 0;

GLuint create_shader_program(RonaGL* gl, const char* vertexSource, const char* fragmentSource);

void text_render_reset(RenderStruct* render_struct);
void text_paragraph(TextParams* text_params, char* text);
void text_printf(TextParams* text_params, char* fmt, ...);
void text_send_to_gpu(RenderStruct* render_struct, RonaGL* gl);

#endif /* RENDERER_H */
