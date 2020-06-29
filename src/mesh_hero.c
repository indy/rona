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

void mesh_hero_lib_load(Mesh *mesh, RonaGl *gl, MemoryArena *transient, Tileset *tileset) {
  gl->genVertexArrays(1, &mesh->vao); // Vertex Array Object
  gl->bindVertexArray(mesh->vao);

#include "../target/shader.vert.c"
  SHADER_AS_STRING(transient, vertexSource, shader_vert);

#include "../target/shader.frag.c"
  SHADER_AS_STRING(transient, fragmentSource, shader_frag);

  mesh->shader_program = create_shader_program(gl, vertexSource, fragmentSource);


  Vec2 sprite = tileset_get_uv(tileset, TS_Hero);
  f32 u = sprite.u;
  f32 v = sprite.v;
  f32 ud = tileset->uv_unit.u;
  f32 vd = tileset->uv_unit.v;

  // clang-format off
  f32 half_dim = 0.35f;
  f32 vertices[] = {
    // positions                     colours                uv
    -half_dim,  half_dim,     1.0f, 1.0f, 1.0f, 1.0f,   u,    v,
    -half_dim, -half_dim,     1.0f, 1.0f, 1.0f, 1.0f,   u,    v+vd,
     half_dim, -half_dim,     1.0f, 1.0f, 1.0f, 1.0f,   u+ud, v+vd,
     half_dim,  half_dim,     1.0f, 1.0f, 1.0f, 1.0f,   u+ud, v
  };
  u32 indices[] = {
    0, 1, 2,
    0, 2, 3
  };
  // clang-format on

  mesh->num_elements = 6;

  // the type of a Vertex Buffer Object is GL_ARRAY_BUFFER
  //
  GLuint vbo;
  gl->genBuffers(1, &vbo);
  gl->bindBuffer(GL_ARRAY_BUFFER, vbo);
  gl->bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                 GL_STATIC_DRAW); // the data is set only once and used many times.
  gl->bindBuffer(GL_ARRAY_BUFFER, 0);

  GLuint ebo;
  gl->genBuffers(1, &ebo);
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  gl->bufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  gl->useProgram(mesh->shader_program);

  mesh->uniform_texture = gl->getUniformLocation(mesh->shader_program, "ourTexture");
  mesh->uniform_colour = gl->getUniformLocation(mesh->shader_program, "colour");
  mesh->uniform_proj_matrix = gl->getUniformLocation(mesh->shader_program, "proj_matrix");
  mesh->uniform_pos = gl->getUniformLocation(mesh->shader_program, "pos");

  gl->bindBuffer(GL_ARRAY_BUFFER, vbo);
  gl->enableVertexAttribArray(0);
  gl->enableVertexAttribArray(1);
  gl->enableVertexAttribArray(2);

  // positions
  gl->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(0 * sizeof(float)));
  // colour
  gl->vertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(2 * sizeof(float)));
  // uv
  gl->vertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(6 * sizeof(float)));

  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  gl->bindVertexArray(0);
}

void mesh_hero_lib_unload(Mesh *mesh, RonaGl *gl) { gl->deleteVertexArrays(1, &mesh->vao); }
