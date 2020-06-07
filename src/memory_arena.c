#include "rona.h"
#include "memory_arena.h"

#include "stdio.h"
#include "platform.h"

u64 kilobytes(u64 s) {
  return (s * 1024);
}

u64 megabytes(u64 s) {
  return kilobytes(s * 1024);
}

u64 gigabytes(u64 s) {
  return megabytes(s * 1024);
}

void *memory_arena_push(MemoryArena *ma, u64 bytes) {
  RONA_ASSERT((ma->used + bytes) <= ma->size)

  void *res = ma->base + ma->used;
  ma->used += bytes;

  return res;
}
