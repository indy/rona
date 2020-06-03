#define FPL_IMPLEMENTATION
#define FPL_NO_VIDEO_SOFTWARE
#define FPL_NO_AUDIO
#include <final_platform_layer.h>

#include "platform.h"
#include <sys/mman.h>

#include <stdbool.h>
#define CR_HOST CR_UNSAFE // try to best manage static states
#include "cr.h"

#include "gfx.h"
#include "game_state.h"

const char *plugin = CR_DEPLOY_PATH "/" CR_PLUGIN(CR_NAME);
const char *tmp = "/tmp/";

static GameState game_state;

#if defined(FPL_PLATFORM_WINDOWS)
static void *GLProcAddress(const char *name) {
  fpl__VideoState *videoState = (fpl__VideoState *)fpl__global__AppState->video.mem;
  fplAssert(videoState != NULL);
  fplAssert(videoState->win32.opengl.api.wglGetProcAddress != NULL);
  void *result = videoState->win32.opengl.api.wglGetProcAddress(name);
  return(result);
}
#else
static void *GLProcAddress(const char *name) {
  fpl__VideoState *videoState = (fpl__VideoState *)fpl__global__AppState->video.mem;
  fplAssert(videoState != NULL);
  fplAssert(videoState->x11.opengl.api.glXGetProcAddress != NULL);
  void *result = videoState->x11.opengl.api.glXGetProcAddress((const GLubyte *)name);
  return(result);
}
#endif

static void LoadGLFunctions(Gfx *gl) {
  gl->clear = (PFNGLCLEARPROC)GLProcAddress("glClear");
  gl->viewport = (PFNGLVIEWPORTPROC)GLProcAddress("glViewport");
  gl->drawArrays = (PFNGLDRAWARRAYSPROC)GLProcAddress("glDrawArrays");
  gl->clearColor = (PFNGLCLEARCOLORPROC)GLProcAddress("glClearColor");
  gl->getIntegerv = (PFNGLGETINTEGERVPROC)GLProcAddress("glGetIntegerv");
  gl->getString = (PFNGLGETSTRINGPROC)GLProcAddress("glGetString");

  gl->createShader = (PFNGLCREATESHADERPROC)GLProcAddress("glCreateShader");
  gl->shaderSource = (PFNGLSHADERSOURCEPROC)GLProcAddress("glShaderSource");
  gl->compileShader = (PFNGLCOMPILESHADERPROC)GLProcAddress("glCompileShader");
  gl->getShaderiv = (PFNGLGETSHADERIVPROC)GLProcAddress("glGetShaderiv");
  gl->attachShader = (PFNGLATTACHSHADERPROC)GLProcAddress("glAttachShader");
  gl->createProgram = (PFNGLCREATEPROGRAMPROC)GLProcAddress("glCreateProgram");
  gl->getShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)GLProcAddress("glGetShaderInfoLog");
  gl->linkProgram = (PFNGLLINKPROGRAMPROC)GLProcAddress("glLinkProgram");
  gl->validateProgram = (PFNGLVALIDATEPROGRAMPROC)GLProcAddress("glValidateProgram");
  gl->getProgramiv = (PFNGLGETPROGRAMIVPROC)GLProcAddress("glGetProgramiv");
  gl->getProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)GLProcAddress("glGetProgramInfoLog");
  gl->deleteShader = (PFNGLDELETESHADERPROC)GLProcAddress("glDeleteShader");
  gl->useProgram = (PFNGLUSEPROGRAMPROC)GLProcAddress("glUseProgram");

  gl->genVertexArrays = (PFNGLGENVERTEXARRAYSPROC)GLProcAddress("glGenVertexArrays");
  gl->bindVertexArray = (PFNGLBINDVERTEXARRAYPROC)GLProcAddress("glBindVertexArray");
  gl->genBuffers = (PFNGLGENBUFFERSPROC)GLProcAddress("glGenBuffers");
  gl->bindBuffer = (PFNGLBINDBUFFERPROC)GLProcAddress("glBindBuffer");
  gl->bufferData = (PFNGLBUFFERDATAPROC)GLProcAddress("glBufferData");
  gl->enableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)GLProcAddress("glEnableVertexAttribArray");
  gl->disableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)GLProcAddress("glDisableVertexAttribArray");
  gl->vertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)GLProcAddress("glVertexAttribPointer");
  gl->deleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)GLProcAddress("glDeleteVertexArrays");
}

void display_cr_failure(cr_plugin* ctx) {
  switch(ctx->failure) {
  case CR_NONE:
    fprintf(stdout, "No error");
    break;
  case CR_SEGFAULT:
    fprintf(stdout, "SIGSEGV / EXCEPTION_ACCESS_VIOLATION");
    break;
  case CR_ILLEGAL:
    fprintf(stdout, "illegal instruction (SIGILL) / EXCEPTION_ILLEGAL_INSTRUCTION");
    break;
  case CR_ABORT:
    fprintf(stdout, "(SIGBRT)");
    break;
  case CR_MISALIGN:
    fprintf(stdout, "bus error (SIGBUS) / EXCEPTION_DATATYPE_MISALIGNMENT");
    break;
  case CR_BOUNDS:
    fprintf(stdout, "EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
    break;
  case CR_STACKOVERFLOW:
    fprintf(stdout, "EXCEPTION_STACK_OVERFLOW");
    break;
  case CR_STATE_INVALIDATED:
    fprintf(stdout, "a failure of cr_plugin_validate_sections");
    break;
  case CR_BAD_IMAGE:
    fprintf(stdout, "The binary is not valid - compiler is still writing it");
    break;
  case CR_INITIAL_FAILURE:
    fprintf(stdout, "Plugin version 1 crashed, cannot rollback");
    break;
  case CR_OTHER:
    fprintf(stdout, "Unknown or other signal");
    break;
  case CR_USER:
    fprintf(stdout, "CR_USER");
    break;
  }
}

size_t kilobytes(size_t s) {
  return (s * 1024);
}

size_t megabytes(size_t s) {
  return kilobytes(s * 1024);
}

size_t gigabytes(size_t s) {
  return megabytes(s * 1024);
}

int main(int argc, char **args) {
  game_state.quit_game = false;
  game_state.game_initialised = false;
  game_state.screen_width = 800;
  game_state.screen_height = 450;

  game_state.mem_size = gigabytes(1);
  game_state.mem_base = mmap(NULL, game_state.mem_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  // printf("mem_base %p\n", game_state.mem_base);
  // *(int *)(game_state.mem_base) = 42;
  // *((int *)(game_state.mem_base) + 1) = 12;

  // int *p = (int *)(game_state.mem_base);
  // printf("%d\n", *p++);
  // printf("%d\n", *p++);

  cr_plugin ctx;
  // the host application should initalize a plugin with a context, a plugin
  // filename without extension and the full path to the plugin
  ctx.userdata = &game_state;
  cr_plugin_open(ctx, plugin);
  cr_set_temporary_path(ctx, tmp);


  // Create default settings
  fplSettings settings = fplMakeDefaultSettings();
  fplCopyString("rona", settings.window.title, fplArrayCount(settings.window.title));

  // Overwrite the video driver
  settings.video.driver = fplVideoDriverType_OpenGL;

  // Modern OpenGL
  settings.video.graphics.opengl.compabilityFlags = fplOpenGLCompabilityFlags_Core;
  settings.video.graphics.opengl.majorVersion = 3;
  settings.video.graphics.opengl.minorVersion = 3;
  settings.video.graphics.opengl.multiSamplingCount = 4;

  if (fplPlatformInit(fplInitFlags_Video, &settings)) {

    LoadGLFunctions(&(game_state.gl));

    const char *version = (const char *)game_state.gl.getString(GL_VERSION);
    const char *vendor = (const char *)game_state.gl.getString(GL_VENDOR);
    const char *renderer = (const char *)game_state.gl.getString(GL_RENDERER);
    RONA_INFO("OpenGL version: %s\n", version);
    RONA_INFO("OpenGL vendor: %s\n", vendor);
    RONA_INFO("OpenGL renderer: %s\n", renderer);

    // Event/Main loop
    while (fplWindowUpdate() && !game_state.quit_game) {
      // Poll events
      fplEvent ev;
      while (fplPollEvent(&ev)) {
        /// ...
      }

      fplWindowSize windowArea;
      fplGetWindowSize(&windowArea);

      game_state.window_width = windowArea.width;
      game_state.window_height = windowArea.height;

      cr_plugin_update(ctx);
      if (ctx.failure != CR_NONE) {
        display_cr_failure(&ctx);
        fflush(stdout);
        fflush(stderr);
        break;
      }

      fplVideoFlip();
    }

    fplPlatformRelease();
    // at the end do not forget to cleanup the plugin context, as it needs to
    // allocate some memory to track internal and plugin states
    cr_plugin_close(ctx);

    return 0;
  } else {
    return -1;
  }
}
