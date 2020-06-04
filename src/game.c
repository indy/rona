#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>

#include "stdio.h"

#include "platform.h"

#include "rona_gl.h"
#include "game_state.h"
#include "game.h"

#include "render.h"

// one time init at startup
void game_startup(GameState* game_state) {
  render_startup(game_state);
}

void game_shutdown(GameState* game_state) {
  render_shutdown(game_state);
}

// changes have been made to the game client and it has now been automatically loaded
void game_lib_load(GameState* game_state) {
  render_lib_load(game_state);
}

// changes have been made to the game client, this old version will be unloaded
void game_lib_unload(GameState* game_state) {
  render_lib_unload(game_state);
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
      printf("%" PRIu64 "\n", game_state->time_delta);
  }

  render_step(game_state);
}
