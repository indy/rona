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

void mesh_lib_load_single_tile(Mesh *mesh, RonaGl *gl, Tileset *tileset,
                               TilesetSprite tile_sprite) {

  mesh->num_elements = 6;
  mesh->shader_type = ShaderType_Tile;

  gl->genVertexArrays(1, &mesh->vao); // Vertex Array Object
  gl->bindVertexArray(mesh->vao);

  Vec2 sprite = tileset_get_uv(tileset, tile_sprite);
  f32  u = sprite.u;
  f32  v = sprite.v;
  f32  ud = tileset->uv_unit.u;
  f32  vd = tileset->uv_unit.v;

  // clang-format off
  f32 half_dim_x = TILE_WIDTH * 0.5f;
  f32 half_dim_y = TILE_HEIGHT * 0.5f;
  f32 vertices[] = {
    //      positions                uv
    -half_dim_x,  half_dim_y,     u,    v,
    -half_dim_x, -half_dim_y,     u,    v+vd,
     half_dim_x, -half_dim_y,     u+ud, v+vd,
     half_dim_x,  half_dim_y,     u+ud, v
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
  gl->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void *)(0 * sizeof(float)));
  // uv
  gl->vertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void *)(2 * sizeof(float)));

  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  gl->bindVertexArray(0);
}

void mesh_lib_unload(Mesh *mesh, RonaGl *gl) { gl->deleteVertexArrays(1, &mesh->vao); }
