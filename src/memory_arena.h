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
