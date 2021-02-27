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

usize kilobytes(usize s) {
  return (s * 1024);
}

usize megabytes(usize s) {
  return kilobytes(s * 1024);
}

usize gigabytes(usize s) {
  return megabytes(s * 1024);
}

void* bump_alloc(BumpAllocator* ba, usize bytes) {
  RONA_ASSERT((ba->used + bytes) <= ba->size)

  void* res = ba->base + ba->used;
  ba->used += bytes;

  return res;
}

void* bump_head(BumpAllocator* ba) {
  return (void*)(ba->base + ba->used);
}

void* memory_block(BumpAllocator* bump, usize bytes_to_allocate, usize bytes_requested) {
  MemoryBlock* block     = (MemoryBlock*)BUMP_ALLOC(bump, bytes_to_allocate);
  block->bytes_allocated = bytes_to_allocate;
  block->bytes_requested = bytes_requested;
  block->next            = NULL;
  return block;
}

void fixed_block_allocator_reset(FixedBlockAllocator* fba, BumpAllocator* bump) {
  fba->bump                   = bump;
  fba->available_one_kilobyte = NULL;
  fba->available_150_kilobyte = NULL;
  fba->available_one_megabyte = NULL;
  fba->available_large        = NULL;
}

void* rona_malloc(FixedBlockAllocator* fba, usize bytes) {
  MemoryBlock* block;

  usize bytes_to_use = bytes + sizeof(MemoryBlock);

  if (bytes_to_use < kilobytes(1)) {
    if (fba->available_one_kilobyte) {
      block                       = fba->available_one_kilobyte;
      fba->available_one_kilobyte = block->next;
      block->next                 = NULL;
      block->bytes_requested      = bytes;
    } else {
      block = memory_block(fba->bump, kilobytes(1), bytes);
    }
  } else if (bytes_to_use < kilobytes(150)) {
    if (fba->available_150_kilobyte) {
      block                       = fba->available_150_kilobyte;
      fba->available_150_kilobyte = block->next;
      block->next                 = NULL;
      block->bytes_requested      = bytes;
    } else {
      block = memory_block(fba->bump, kilobytes(150), bytes);
    }
  } else if (bytes_to_use < megabytes(1)) {
    if (fba->available_one_megabyte) {
      block                       = fba->available_one_megabyte;
      fba->available_one_megabyte = block->next;
      block->next                 = NULL;
      block->bytes_requested      = bytes;
    } else {
      block = memory_block(fba->bump, megabytes(1), bytes);
    }
  } else {
    // stupid simple method of just looking at the head of the list to see if it's big enough
    if (fba->available_large && fba->available_large->bytes_allocated > bytes_to_use) {
      block                  = fba->available_large;
      fba->available_large   = block->next;
      block->next            = NULL;
      block->bytes_requested = bytes;
    } else {
      block = memory_block(fba->bump, bytes_to_use, bytes);
    }
  }

  return (void*)((byte*)block + sizeof(MemoryBlock));
}

void* rona_realloc(FixedBlockAllocator* fba, void* mem, usize bytes) {
  if (!mem) {
    return rona_malloc(fba, bytes);
  }

  MemoryBlock* block = (MemoryBlock*)((byte*)mem - sizeof(MemoryBlock));

  usize bytes_to_use = bytes + sizeof(MemoryBlock);

  if (block->bytes_allocated > bytes_to_use) {
    // the existing memory block is large enough
    block->bytes_requested = bytes;
    return mem;
  }

  void* new_mem = rona_malloc(fba, bytes);
  memcpy(new_mem, mem, block->bytes_requested);

  rona_free(fba, mem);

  return new_mem;
}

void rona_free(FixedBlockAllocator* fba, void* mem) {
  if (!mem) {
    return;
  }
  MemoryBlock* block = (MemoryBlock*)((byte*)mem - sizeof(MemoryBlock));

  if (block->bytes_allocated <= kilobytes(1)) {
    block->next                 = fba->available_one_kilobyte;
    fba->available_one_kilobyte = block;
  } else if (block->bytes_allocated <= kilobytes(150)) {
    block->next                 = fba->available_150_kilobyte;
    fba->available_150_kilobyte = block;
  } else if (block->bytes_allocated <= megabytes(1)) {
    block->next                 = fba->available_one_megabyte;
    fba->available_one_megabyte = block;
  } else {
    block->next          = fba->available_large;
    fba->available_large = block;
  }
}

void* rona_permanent_malloc(usize bytes) {
  void* addr;
#ifdef SYS_MALLOC
  addr = malloc(bytes);
#else
  addr = rona_malloc(&(g_game_state->allocator_permanent), bytes);
#endif

#ifdef DEBUG_MALLOC
  RONA_LOG("called malloc %d bytes -> %p\n", bytes, addr);
#endif

  return addr;
}

void* rona_permanent_realloc(void* mem, usize bytes) {
  void* addr;
#ifdef SYS_MALLOC
  addr = realloc(mem, bytes);
#else
  addr = rona_realloc(&(g_game_state->allocator_permanent), mem, bytes);
#endif

#ifdef DEBUG_MALLOC
  RONA_LOG("called realloc %p to %d bytes -> %p\n", mem, bytes, addr);
#endif

  return addr;
}

void rona_permanent_free(void* mem) {
#ifdef DEBUG_MALLOC
  RONA_LOG("called free %p\n", mem);
#endif
#ifdef SYS_MALLOC
  free(mem);
#else
  rona_free(&(g_game_state->allocator_permanent), mem);
#endif
}

void* rona_transient_malloc(usize bytes) {
  void* addr;
#ifdef SYS_MALLOC
  addr = malloc(bytes);
#else
  addr = rona_malloc(&(g_game_state->allocator_transient), bytes);
#endif

#ifdef DEBUG_MALLOC
  RONA_LOG("called malloc %d bytes -> %p\n", bytes, addr);
#endif

  return addr;
}

void* rona_transient_realloc(void* mem, usize bytes) {
  void* addr;
#ifdef SYS_MALLOC
  addr = realloc(mem, bytes);
#else
  addr = rona_realloc(&(g_game_state->allocator_transient), mem, bytes);
#endif

#ifdef DEBUG_MALLOC
  RONA_LOG("called realloc %p to %d bytes -> %p\n", mem, bytes, addr);
#endif

  return addr;
}

void rona_transient_free(void* mem) {
#ifdef DEBUG_MALLOC
  RONA_LOG("called free %p\n", mem);
#endif
#ifdef SYS_MALLOC
  free(mem);
#else
  rona_free(&(g_game_state->allocator_transient), mem);
#endif
}
