#ifndef GAME_H
#define GAME_H

#ifdef _DEBUG
#define ASSERT(exp)        \
  if(!(exp)) { \
    fprintf(stderr, "ASSERT FAILURE: %s (%s:%d)\n", __func__, __FILE__, __LINE__); \
    fflush(stderr); \
    *(int *)0 = 0; \
  }
#else
#define ASSERT(exp) {}
#endif

void game_init(HostData* h);
void game_shutdown(HostData* h);
void game_tick(HostData* h);

#endif /* GAME_H */
