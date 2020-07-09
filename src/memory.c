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
  MemoryBlock* block = (MemoryBlock*)BUMP_ALLOC(bump, bytes_to_allocate);
  block->bytes_allocated = bytes_to_allocate;
  block->bytes_requested = bytes_requested;
  block->next = NULL;
  return block;
}

void grouped_allocator_reset(GroupedAllocator* ga, BumpAllocator* bump) {
  ga->bump = bump;
  ga->available_one_kilobyte = NULL;
  ga->available_150_kilobyte = NULL;
  ga->available_one_megabyte = NULL;
  ga->available_large = NULL;
}

void* rona_malloc(GroupedAllocator* ga, usize bytes) {
  MemoryBlock* block;

  usize bytes_to_use = bytes + sizeof(MemoryBlock);

  if (bytes_to_use < kilobytes(1)) {
    if (ga->available_one_kilobyte) {
      block = ga->available_one_kilobyte;
      ga->available_one_kilobyte = block->next;
      block->next = NULL;
      block->bytes_requested = bytes;
    } else {
      block = memory_block(ga->bump, kilobytes(1), bytes);
    }
  } else if (bytes_to_use < kilobytes(150)) {
    if (ga->available_150_kilobyte) {
      block = ga->available_150_kilobyte;
      ga->available_150_kilobyte = block->next;
      block->next = NULL;
      block->bytes_requested = bytes;
    } else {
      block = memory_block(ga->bump, kilobytes(150), bytes);
    }
  } else if (bytes_to_use < megabytes(1)) {
    if (ga->available_one_megabyte) {
      block = ga->available_one_megabyte;
      ga->available_one_megabyte = block->next;
      block->next = NULL;
      block->bytes_requested = bytes;
    } else {
      block = memory_block(ga->bump, megabytes(1), bytes);
    }
  } else {
    // stupid simple method of just looking at the head of the list to see if it's big enough
    if (ga->available_large && ga->available_large->bytes_allocated > bytes_to_use) {
      block = ga->available_large;
      ga->available_large = block->next;
      block->next = NULL;
      block->bytes_requested = bytes;
    } else {
      block = memory_block(ga->bump, bytes_to_use, bytes);
    }
  }

  return (void*)((byte*)block + sizeof(MemoryBlock));
}

void* rona_realloc(GroupedAllocator* ga, void* mem, usize bytes) {
  if (!mem) {
    return rona_malloc(ga, bytes);
  }

  MemoryBlock* block = (MemoryBlock*)((byte*)mem - sizeof(MemoryBlock));

  usize bytes_to_use = bytes + sizeof(MemoryBlock);

  if (block->bytes_allocated > bytes_to_use) {
    // the existing memory block is large enough
    block->bytes_requested = bytes;
    return mem;
  }

  void* new_mem = rona_malloc(ga, bytes);
  memcpy(new_mem, mem, block->bytes_requested);

  rona_free(ga, mem);

  return new_mem;
}

void rona_free(GroupedAllocator* ga, void* mem) {
  if (!mem) {
    return;
  }
  MemoryBlock* block = (MemoryBlock*)((byte*)mem - sizeof(MemoryBlock));

  if (block->bytes_allocated <= kilobytes(1)) {
    block->next = ga->available_one_kilobyte;
    ga->available_one_kilobyte = block;
  } else if (block->bytes_allocated <= kilobytes(150)) {
    block->next = ga->available_150_kilobyte;
    ga->available_150_kilobyte = block;
  } else if (block->bytes_allocated <= megabytes(1)) {
    block->next = ga->available_one_megabyte;
    ga->available_one_megabyte = block;
  } else {
    block->next = ga->available_large;
    ga->available_large = block;
  }
}
