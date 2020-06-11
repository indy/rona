#ifndef LEVEL_1_H
#define LEVEL_1_H

void level1_startup(Level *level, GameState *game_state);
void level1_shutdown(Level *level);

void level1_lib_load(Level *level, RonaGl *gl, MemoryArena *transient);
void level1_lib_unload(Level *level, RonaGl *gl);

#endif /* LEVEL_1_H */
