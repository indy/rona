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

usize kilobytes(usize s) { return (s * 1024); }

usize megabytes(usize s) { return kilobytes(s * 1024); }

usize gigabytes(usize s) { return megabytes(s * 1024); }

void *arena_alloc(MemoryArena *ma, usize bytes) {
  RONA_ASSERT((ma->used + bytes) <= ma->size)

  void *res = ma->base + ma->used;
  ma->used += bytes;

  // byte *b = (byte *)res;
  // for (usize i=0; i < bytes; i++) {
  //   *b++ = 0;
  // }

  return res;
}

void *memory_block(MemoryArena *arena, usize bytes_to_allocate, usize bytes_requested) {
  MemoryBlock *block = (MemoryBlock *)ARENA_ALLOC(arena, bytes_to_allocate);
  block->bytes_allocated = bytes_to_allocate;
  block->bytes_requested = bytes_requested;
  block->next = NULL;
  return block;
}

void memory_allocator_reset(MemoryAllocator *ma, MemoryArena *arena) {
  ma->arena = arena;
  ma->available_1k = NULL;
  ma->available_10k = NULL;
  ma->available_large = NULL;
}

void *rona_malloc(MemoryAllocator *ma, usize bytes) {
  MemoryBlock *block;

  usize bytes_to_use = bytes + sizeof(MemoryBlock);
  if (bytes_to_use < kilobytes(1)) {
    if (ma->available_1k) {
      block = ma->available_1k;
      ma->available_1k = block->next;
      block->next = NULL;
      block->bytes_requested = bytes;
    } else {
      block = memory_block(ma->arena, kilobytes(1), bytes);
    }
  } else if (bytes_to_use < kilobytes(10)) {
    if (ma->available_10k) {
      block = ma->available_10k;
      ma->available_10k = block->next;
      block->next = NULL;
      block->bytes_requested = bytes;
    } else {
      block = memory_block(ma->arena, kilobytes(10), bytes);
    }
  } else {
    if (ma->available_large) {
      block = ma->available_large;
      ma->available_large = block->next;
      block->next = NULL;
      block->bytes_requested = bytes;
    } else {
      block = memory_block(ma->arena, bytes_to_use, bytes);
    }
  }

  return (void *)((byte *)block + sizeof(MemoryBlock));
}

void rona_free(MemoryAllocator *ma, void* mem) {
  if (!mem) {
    return;
  }
  MemoryBlock *block = (MemoryBlock *)((byte *)mem - sizeof(MemoryBlock));

  if (block->bytes_allocated <= kilobytes(1)) {
    block->next = ma->available_1k;
    ma->available_1k = block;
  } else if (block->bytes_allocated <= kilobytes(10)) {
    block->next = ma->available_10k;
    ma->available_10k = block;
  } else {
    block->next = ma->available_large;
    ma->available_large = block;
  }
}

void *rona_realloc(MemoryAllocator *ma, void* mem, usize bytes) {
  if (!mem) {
    return rona_malloc(ma, bytes);
  }

  MemoryBlock *block = (MemoryBlock *)((byte *)mem - sizeof(MemoryBlock));

  usize bytes_to_use = bytes + sizeof(MemoryBlock);

  if (block->bytes_allocated > bytes_to_use) {
    // the existing memory block is large enough
    block->bytes_requested = bytes;
    return mem;
  }

  void *new_mem = rona_malloc(ma, bytes);
  memcpy(new_mem, mem, block->bytes_requested);

  rona_free(ma, mem);

  return new_mem;
}
