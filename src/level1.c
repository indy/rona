#include "stdio.h"
#include "rona.h"
#include "platform.h"
#include "game_state.h"
#include "level1.h"
#include "memory_arena.h"

// todo: game_state only required for meshes, replace with something better

void level1_startup(Level *level, GameState *game_state) {
  level->width = 7;
  level->height = 5;

  level->max_num_entities = 10;
  level->entities = (Entity *)ARENA_ALLOC(&(level->mem), sizeof(Entity) * level->max_num_entities);
  for (i32 i = 0; i < level->max_num_entities; i++) {
    level->entities[i].exists = false;
  }

  Entity *hero = &(level->entities[0]);

  hero->entity_type = EntityType_Hero;
  hero->entity_state = EntityState_Standing;
  hero->exists = true;

  hero->board_pos.x = 1;
  hero->board_pos.y = 1;
  hero->world_pos.x = 1.0f;
  hero->world_pos.y = 1.0f;
  hero->world_target.x = 1.0f;
  hero->world_target.y = 1.0f;
  hero->mesh = game_state->mesh_hero;

  hero->colour.r = 0.9f;
  hero->colour.g = 0.1f;
  hero->colour.b = 0.2f;
  hero->colour.a = 1.0f;
}

void level1_shutdown(Level *level) {
  level->mem.used = 0;
}
