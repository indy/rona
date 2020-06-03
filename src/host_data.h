#ifndef HOST_DATA_H
#define HOST_DATA_H


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
  } HostData;

#ifdef __cplusplus
}
#endif

#endif /* HOST_DATA_H */
