#ifndef GAME_STATE_H
#define GAME_STATE_H


#ifdef __cplusplus
extern "C" {
#endif

  typedef struct  {
    bool game_initialised;
    bool quit_game;

    int screen_width;
    int screen_height;

    int window_width;
    int window_height;

    size_t mem_size;
    void *mem_base;

    GLuint vertexArrayID;

    Gfx gl;
  } GameState;

#ifdef __cplusplus
}
#endif

#endif /* GAME_STATE_H */
