#ifndef GAME_STATE_H
#define GAME_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rona_input.h"

  typedef struct  {
    bool game_initialised;
    bool quit_game;

    int screen_width;
    int screen_height;

    bool window_resized;
    int window_width;
    int window_height;

    uint64_t time_game_start;   // time at the start of the game
    uint64_t time_last_frame;
    uint64_t time_delta;

    size_t mem_size;
    void *mem_base;

    GLuint vertex_array_id;

    RonaGl gl;
    RonaInput input;
  } GameState;

#ifdef __cplusplus
}
#endif

#endif /* GAME_STATE_H */
