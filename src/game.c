#include "stdio.h"
#include "rona.h"
#include "platform.h"
#include "game_state.h"
#include "game.h"
#include "renderer.h"
#include "memory_arena.h"
#include "input.h"
#include "mesh_hero.h"
#include "level1.h"

// one time init at startup
void game_startup(GameState* game_state) {
  game_state->mesh_hero = (Mesh *)ARENA_ALLOC(&game_state->storage_permanent, sizeof(Mesh));

  u64 level_memory_arena_size = megabytes(64);
  game_state->level = (Level *)ARENA_ALLOC(&game_state->storage_permanent, level_memory_arena_size);
  game_state->level->mem.base = game_state->level + sizeof(Level);
  game_state->level->mem.size = level_memory_arena_size - sizeof(Level);
  game_state->level->mem.used = 0;

  level1_startup(game_state->level, game_state);

  renderer_startup(game_state->gl);
}

void game_shutdown(GameState* game_state) {
  level1_shutdown(game_state->level);
  renderer_shutdown(game_state->gl);
}

// changes have been made to the game client and it has now been automatically loaded
void game_lib_load(GameState* game_state) {
  renderer_lib_load(game_state->gl);
  mesh_hero_lib_load(game_state->mesh_hero, game_state->gl, &game_state->storage_transient);
}

// changes have been made to the game client, this old version will be unloaded
void game_lib_unload(GameState* game_state) {
  mesh_hero_lib_unload(game_state->mesh_hero, game_state->gl);
  renderer_lib_unload(game_state->gl);
}


void game_step(GameState* game_state) {
  game_state->storage_transient.used = 0;

  if (key_down(game_state->input, Key_Escape) ||
      key_down(game_state->input, Key_Q)) {
    game_state->quit_game = true;
    return;
  }
  if (key_pressed(game_state->input, Key_T)) {
    RONA_OUT("press");
    fflush(stdout);
  }


  renderer_render(game_state->gl, game_state->level, game_state->window_width, game_state->window_height);
}
