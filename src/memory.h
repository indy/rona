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

#ifndef MEMORY_H
#define MEMORY_H

usize kilobytes(usize s);
usize megabytes(usize s);
usize gigabytes(usize s);

void* bump_alloc(BumpAllocator* ba, usize bytes);
void* bump_head(BumpAllocator* ba);

#define BUMP_ALLOC(bump, size) bump_alloc((bump), (size));

void  grouped_allocator_reset(FixedBlockAllocator* fba, BumpAllocator* bump);
void* rona_malloc(FixedBlockAllocator* fba, usize bytes);
void  rona_free(FixedBlockAllocator* fba, void* mem);
void* rona_realloc(FixedBlockAllocator* fba, void* mem, usize bytes);

void* rona_permanent_malloc(usize bytes);
void* rona_permanent_realloc(void* mem, usize bytes);
void  rona_permanent_free(void* mem);
void* rona_transient_malloc(usize bytes);
void* rona_transient_realloc(void* mem, usize bytes);
void  rona_transient_free(void* mem);

#endif /* MEMORY_H */
