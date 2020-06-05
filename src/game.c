#include "rona.h"
#include "platform.h"
#include "game.h"
#include "renderer.h"

// one time init at startup
void game_startup(GameState* game_state) {
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
  if (game_state->input.key[Key_Escape] == ButtonState_Press) {
    game_state->quit_game = true;
    return;
  }
  if (game_state->input.key[Key_Q] == ButtonState_Press) {
    game_state->quit_game = true;
    return;
  }
  if (game_state->input.key[Key_T] == ButtonState_Press) {
    //      printf("%" PRIu64 "\n", game_state->time_delta);
  }

  renderer_step(game_state);
}
