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

void mesh_screen_lib_load(Mesh *mesh, RonaGl *gl, MemoryArena *transient) {
  gl->genVertexArrays(1, &mesh->vao); // Vertex Array Object
  gl->bindVertexArray(mesh->vao);

  #include "../target/screen.vert.c"
  SHADER_AS_STRING(transient, vertexSource, screen_vert);

  #include "../target/screen.frag.c"
  SHADER_AS_STRING(transient, fragmentSource, screen_frag);

  mesh->shader_program = create_shader_program(gl, vertexSource, fragmentSource);

  // x, y, u, v
  f32 vertices[] = {
    0.0f, 360.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    640.0f, 0.0f, 1.0f, 0.0f,
    640.0f, 360.0f, 1.0f, 1.0f
  };
  u32 indices[] = {
    0, 1, 2,
    0, 2, 3
  };

  mesh->num_elements = 6;

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

  gl->useProgram(mesh->shader_program);

  mesh->uniform_texture = gl->getUniformLocation(mesh->shader_program, "sampler");
  mesh->uniform_proj_matrix = gl->getUniformLocation(mesh->shader_program, "proj_matrix");

  gl->bindBuffer(GL_ARRAY_BUFFER, vbo);
  gl->enableVertexAttribArray(0);
  gl->enableVertexAttribArray(1);

  // positions
  gl->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, NULL);

  // texture coords
  gl->vertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid *)((sizeof(float) * 2)));

  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  gl->bindVertexArray(0);
}

void mesh_screen_lib_unload(Mesh *mesh, RonaGl *gl) {
  gl->deleteVertexArrays(1, &mesh->vao);
}