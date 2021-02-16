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

#ifndef CHUNK_H
#define CHUNK_H

ChunkPos chunk_pos_from_stage_coords(Vec2i stage_coords, Vec2i viewport);
void     chunk_pos_log(char* msg, ChunkPos cp);
#define  CHUNKPOS_LOG(x) chunk_pos_log(#x,x)

// gets the chunk specified by cp, creating a chunk if it doesn't exist
Chunk* chunk_ensure_get(Level* level, Vec2i chunk_pos);

Chunk* chunk_get(Chunk* chunks, Vec2i chunk_pos);

ChunkTile* chunktile_ensure_get(Level* level, ChunkPos cp);

#endif // CHUNK_H
