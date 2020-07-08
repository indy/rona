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

#ifndef MESH_H
#define MESH_H

// void mesh_hero_render(Mesh *mesh, RonaGL *gl, Mat4 *proj_matrix);
void mesh_lib_load_single_tile(Mesh* mesh, RonaGL* gl, Tileset* tileset, TilesetSprite tile_sprite,
                               Colour fg_col, Colour bg_col);
void mesh_lib_unload(Mesh* mesh, RonaGL* gl);

#endif /* MESH_H */
