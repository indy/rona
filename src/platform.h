#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _DEBUG
#define RONA_ASSERT(exp)        \
  if(!(exp)) { \
    fprintf(stderr, "ASSERT FAILURE: %s (%s:%d)\n", __func__, __FILE__, __LINE__); \
    fflush(stderr); \
    *(int *)0 = 0; \
  }
#else
#define RONA_ASSERT(exp) {}
#endif

#define RONA_ERROR(...) fprintf(stderr, ##__VA_ARGS__)
#define RONA_INFO(...) fprintf(stdout, ##__VA_ARGS__)
#define RONA_OUT(f_) fprintf(stdout, (f_))



#endif
