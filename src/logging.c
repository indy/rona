#include <stdarg.h>

void rona_error_implementation(char* file_location, int line_num, char* function, char* fmt, ...) {
  fprintf(stderr, "ERROR [%s:%d %s]: ", file_location, line_num, function);
  va_list argptr;
  va_start(argptr, fmt);
  vfprintf(stderr, fmt, argptr);
  va_end(argptr);
  fprintf(stderr, "\n");
}

void rona_info_implementation(char* file_location, int line_num, char* function, char* fmt, ...) {
  fprintf(stdout, "INFO [%s:%d %s]: ", file_location, line_num, function);
  va_list argptr;
  va_start(argptr, fmt);
  vfprintf(stdout, fmt, argptr);
  va_end(argptr);
  fprintf(stdout, "\n");
}

void rona_log_implementation(char* file_location, int line_num, char* function, char* fmt, ...) {
  fprintf(stdout, "LOG [%s:%d %s]: ", file_location, line_num, function);
  va_list argptr;
  va_start(argptr, fmt);
  vfprintf(stdout, fmt, argptr);
  va_end(argptr);
  fprintf(stdout, "\n");
}
