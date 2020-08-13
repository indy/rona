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

void mesh_lib_load_single_tile(Mesh* mesh, RonaGL* gl, Tileset* tileset, TilesetSprite tile_sprite,
                               Colour fg_col, Colour bg_col) {

  Vec4   fg, bg;
  Colour c;
  vec4_from_colour(&fg, colour_clone_as(&c, &fg_col, ColourFormat_RGB));
  vec4_from_colour(&bg, colour_clone_as(&c, &bg_col, ColourFormat_RGB));

  mesh->num_elements = 6;
  mesh->shader_type = ShaderType_Tile;

  gl->genVertexArrays(1, &mesh->vao); // Vertex Array Object

  // RONA_LOG("mesh vao %d\n", mesh->vao);

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
    //      positions                uv             foreground colour            background colour
    -half_dim_x, -half_dim_y,     u,    v,     fg.e[0], fg.e[1], fg.e[2], fg.e[3],  bg.e[0], bg.e[1], bg.e[2], bg.e[3],
    -half_dim_x,  half_dim_y,     u,    v+vd,  fg.e[0], fg.e[1], fg.e[2], fg.e[3],  bg.e[0], bg.e[1], bg.e[2], bg.e[3],
     half_dim_x,  half_dim_y,     u+ud, v+vd,  fg.e[0], fg.e[1], fg.e[2], fg.e[3],  bg.e[0], bg.e[1], bg.e[2], bg.e[3],
     half_dim_x, -half_dim_y,     u+ud, v,     fg.e[0], fg.e[1], fg.e[2], fg.e[3],  bg.e[0], bg.e[1], bg.e[2], bg.e[3]
  };
  u32 indices[] = {
    0, 1, 2,
    0, 2, 3
  };
  // clang-format on

  // the type of a Vertex Buffer Object is GL_ARRAY_BUFFER
  //
  gl->genBuffers(1, &mesh->vbo);
  gl->bindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
  gl->bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                 GL_STATIC_DRAW); // the data is set only once and used many times.

  gl->enableVertexAttribArray(0);
  gl->enableVertexAttribArray(1);
  gl->enableVertexAttribArray(2);
  gl->enableVertexAttribArray(3);

  u32 num_floats = TILED_VERTEX_NUM_FLOATS_FOR_GEOMETRY;

  // positions
  gl->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * num_floats,
                          (void*)(0 * sizeof(float)));
  // uv
  gl->vertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * num_floats,
                          (void*)(2 * sizeof(float)));
  // fg colour
  gl->vertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(float) * num_floats,
                          (void*)(4 * sizeof(float)));
  // bg colour
  gl->vertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(float) * num_floats,
                          (void*)(8 * sizeof(float)));

  gl->genBuffers(1, &mesh->ebo);
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
  gl->bufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  gl->bindVertexArray(0);
}

void mesh_lib_unload(Mesh* mesh, RonaGL* gl) {
  gl->deleteVertexArrays(1, &mesh->vao);
}
