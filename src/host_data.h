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

    size_t mem_size;
    void *mem_base;


    GLuint program;
    GLint mvp_location;
    GLuint vertex_array;

  } HostData;

#ifdef __cplusplus
}
#endif

#endif /* HOST_DATA_H */
