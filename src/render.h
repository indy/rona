#ifndef RENDER_H
#define RENDER_H

void render_startup(GameState *game_state);
void render_shutdown(GameState *game_state);

void render_lib_load(GameState *game_state);
void render_lib_unload(GameState *game_state);

void render_step(GameState *game_state);


#endif /* RENDER_H */
