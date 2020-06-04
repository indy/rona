#ifndef GAME_H
#define GAME_H

void game_startup(GameState* game_state);
void game_shutdown(GameState* game_state);

void game_lib_load(GameState* game_state);
void game_lib_unload(GameState* game_state);

void game_step(GameState* game_state);

#endif /* GAME_H */
