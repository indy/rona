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

#ifndef MEMORY_ARENA_H
#define MEMORY_ARENA_H

u64 kilobytes(u64 s);
u64 megabytes(u64 s);
u64 gigabytes(u64 s);

void *arena_alloc(MemoryArena *ma, u64 bytes);

#define ARENA_ALLOC(arena, size) \
  arena_alloc((arena), (size));

// define a MemoryArena in the the transient storage
//
#define MA_TRANSIENT(new_arena, game_state) \
  MemoryArena new_arena; \
  new_arena.base = game_state->storage_transient.base + game_state->storage_transient.used; \
  new_arena.size = game_state->storage_transient.size - game_state->storage_transient.used; \
  new_arena.used = 0;


#endif /* MEMORY_ARENA_H */
