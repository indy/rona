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

#ifndef GRAPHIC_SCREEN_H
#define GRAPHIC_SCREEN_H

// void graphic_screen_render(Graphic *graphic, RonaGL *gl, Mat4 *proj_matrix);
void graphic_screen_lib_load(Graphic* graphic, RonaGL* gl, RenderStruct* render_struct);
void graphic_screen_lib_unload(Graphic* graphic, RonaGL* gl);

#endif /* GRAPHIC_SCREEN_H */
