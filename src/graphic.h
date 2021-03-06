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

#ifndef GRAPHIC_H
#define GRAPHIC_H

void graphic_allocate_mesh(Graphic* graphic, BumpAllocator* bump_allocator, u32 bytes_to_allocate);

void graphic_setup_for_quads(Graphic* graphic, RonaGL* gl, BumpAllocator* transient, u32 max_quads_to_render);
void graphic_setup_screen(Graphic* graphic, RonaGL* gl, f32 stage_width, f32 stage_height);

void graphic_teardown(Graphic* graphic, RonaGL* gl);

void graphic_entities_regenerate_geometry(Level* level, RonaGL* gl, RenderStruct* render_struct);
void graphic_tile_regenerate_geometry(Level* level, RonaGL* gl, RenderStruct* render_struct);

#endif /* GRAPHIC_H */
