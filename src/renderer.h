#ifndef RENDERER_H
#define RENDERER_H

void renderer_startup(GameState *game_state);
void renderer_shutdown(GameState *game_state);

void renderer_lib_load(GameState *game_state);
void renderer_lib_unload(GameState *game_state);

void renderer_step(GameState *game_state);


#endif /* RENDERER_H */
