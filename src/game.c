#include "stdio.h"
#include "rona.h"
#include "platform.h"
#include "game.h"
#include "renderer.h"
#include "memory_arena.h"
#include "input.h"

// one time init at startup
void game_startup(GameState* game_state) {
  u64 level_memory_arena_size = megabytes(64);

  game_state->pLevel = (Level *)MEM_PUSH(&game_state->storage_permanent, level_memory_arena_size);
  game_state->pLevel->mem.base = game_state->pLevel + sizeof(Level);
  game_state->pLevel->mem.size = level_memory_arena_size - sizeof(Level);
  game_state->pLevel->mem.used = 0;

  renderer_startup(game_state);
}

void game_shutdown(GameState* game_state) {
  renderer_shutdown(game_state);
}

// changes have been made to the game client and it has now been automatically loaded
void game_lib_load(GameState* game_state) {
  renderer_lib_load(game_state);
}

// changes have been made to the game client, this old version will be unloaded
void game_lib_unload(GameState* game_state) {
  renderer_lib_unload(game_state);
}


void game_step(GameState* game_state) {
  game_state->storage_transient.used = 0;

  if (key_down(&game_state->input, Key_Escape) ||
      key_down(&game_state->input, Key_Q)) {
    game_state->quit_game = true;
    return;
  }
  if (key_pressed(&game_state->input, Key_T)) {
    RONA_OUT("press");
    fflush(stdout);
  }


  renderer_step(game_state);
}
