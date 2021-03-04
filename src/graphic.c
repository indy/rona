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

void graphic_allocate_mesh(Graphic* graphic, BumpAllocator* bump_allocator, u32 bytes_to_allocate) {
  graphic->mesh                      = (f32*)BUMP_ALLOC(bump_allocator, bytes_to_allocate);
  graphic->mesh_allocated_size_bytes = bytes_to_allocate;
  graphic->sizeof_vbo                = 0;
  graphic->num_elements              = 0;
}

void graphic_setup_for_quads(Graphic* graphic, RonaGL* gl, BumpAllocator* transient,
                             u32 max_quads_to_render) {
  gl->genVertexArrays(1, &graphic->vao); // Vertex Array Object
  gl->bindVertexArray(graphic->vao);

  graphic->num_elements = 0;
  i32 stride            = TILED_QUAD_NUM_FLOATS;

  graphic->sizeof_vbo = sizeof(f32) * stride * max_quads_to_render;
  u32  sizeof_indices = sizeof(u32) * 6 * max_quads_to_render;
  u32* indices        = (u32*)BUMP_ALLOC(transient, sizeof_indices);

  // build indices for geometry

  // clang-format off
  for (i32 tile_count = 0; tile_count < max_quads_to_render; tile_count++) {
    i32 i_index = tile_count * 6;
    i32 offset = tile_count * 4;
    indices[i_index + 0] = 0 + offset;
    indices[i_index + 1] = 1 + offset;
    indices[i_index + 2] = 2 + offset;
    indices[i_index + 3] = 0 + offset;
    indices[i_index + 4] = 2 + offset;
    indices[i_index + 5] = 3 + offset;
  }
  // clang-format on

  // the type of a Vertex Buffer Object is GL_ARRAY_BUFFER
  //
  gl->genBuffers(1, &graphic->vbo);
  gl->bindBuffer(GL_ARRAY_BUFFER, graphic->vbo);
  gl->bufferData(GL_ARRAY_BUFFER, graphic->sizeof_vbo, 0,
                 GL_DYNAMIC_DRAW); // the data is set only once and used many times.
  // gl->bindBuffer(GL_ARRAY_BUFFER, 0);

  gl->genBuffers(1, &graphic->ebo);
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, graphic->ebo);
  gl->bufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof_indices, indices, GL_DYNAMIC_DRAW);
  // gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  gl->bindBuffer(GL_ARRAY_BUFFER, graphic->vbo);
  gl->enableVertexAttribArray(0);
  gl->enableVertexAttribArray(1);

  // positions X Y
  gl->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * TILED_VERTEX_NUM_FLOATS,
                          (void*)(0 * sizeof(float)));
  // texture U V
  gl->vertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * TILED_VERTEX_NUM_FLOATS,
                          (void*)(2 * sizeof(float)));

  // gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, graphic->ebo);

  gl->bindVertexArray(0);
}

void graphic_setup_screen(Graphic* graphic, RonaGL* gl, f32 stage_width, f32 stage_height) {
  graphic->num_elements = 6;
  graphic->shader_type  = ShaderType_Screen;

  gl->genVertexArrays(1, &graphic->vao); // Vertex Array Object
  gl->bindVertexArray(graphic->vao);

  // clang-format off
  // x, y, u, v
  f32 vertices[] = {
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, stage_height, 0.0f, 1.0f,
    stage_width, stage_height, 1.0f, 1.0f,
    stage_width, 0.0f, 1.0f, 0.0f
  };
  u32 indices[] = {
    0, 1, 2,
    0, 2, 3
  };
  // clang-format on

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

  gl->bindBuffer(GL_ARRAY_BUFFER, vbo);
  gl->enableVertexAttribArray(0);
  gl->enableVertexAttribArray(1);

  // positions
  gl->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, NULL);

  // texture coords
  gl->vertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)((sizeof(float) * 2)));

  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  gl->bindVertexArray(0);
}

void graphic_teardown(Graphic* graphic, RonaGL* gl) {
  gl->deleteVertexArrays(1, &graphic->vao);
}
