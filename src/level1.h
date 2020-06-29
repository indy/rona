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

#ifndef LEVEL_1_H
#define LEVEL_1_H

void level1_startup(Level *level, GameState *game_state);
void level1_shutdown(Level *level);

void level1_lib_load(Level *level, RonaGl *gl, MemoryArena *transient, Tileset *tileset);
void level1_lib_unload(Level *level, RonaGl *gl);

#endif /* LEVEL_1_H */
