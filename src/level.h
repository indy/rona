#ifndef LEVEL_H
#define LEVEL_H

bool is_valid_move_direction(Level *level, Entity *entity, i32 x, i32 y);
void level_build(GameState *game_state, Level *level, i32 dbl_width, i32 height, char layout[][dbl_width]);

void mesh_floor_lib_load(Level *level, RonaGl *gl, MemoryArena *transient);
void mesh_floor_lib_unload(Level *level, RonaGl *gl);

#endif /* LEVEL_H */
