#include "rona.h"
#include "platform.h"
#include "game_state.h"
#include "level.h"
#include "level1.h"
//#include "memory_arena.h"


void level1_startup(Level *level, GameState *game_state) {
  char layout[5][14] = {
    ". . . . . .   ",
    ". . .   . .   ",
    ". . .   . .   ",
    ". H . . . . . ",
    ". . . . . . . "
  };

  level_build(game_state, level, 14, 5, layout);
}

void level1_shutdown(Level *level) {
  level->mem.used = 0;
}

void level1_lib_load(Level *level, RonaGl *gl, MemoryArena *transient) {
  mesh_floor_lib_load(level, gl, transient);
}
void level1_lib_unload(Level *level, RonaGl *gl) {
  mesh_floor_lib_unload(level, gl);
}
