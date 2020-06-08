#define FPL_IMPLEMENTATION
#define FPL_NO_VIDEO_SOFTWARE
#define FPL_NO_AUDIO
#include <final_platform_layer.h>

// for access function used to check if plugin exists
#include "unistd.h"

#include <sys/mman.h>

#define CR_HOST CR_UNSAFE // try to best manage static states
#include "cr.h"

typedef unsigned long long u64; // copied from rona.h

#include "../rona.h"
#include "../platform.h"

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

static void LoadGLFunctions(RonaGl *gl) {
  gl->clear = (PFNGLCLEARPROC)GLProcAddress("glClear");
  gl->viewport = (PFNGLVIEWPORTPROC)GLProcAddress("glViewport");
  gl->drawArrays = (PFNGLDRAWARRAYSPROC)GLProcAddress("glDrawArrays");
  gl->drawElements = (PFNGLDRAWELEMENTSPROC)GLProcAddress("glDrawElements");
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
  game_state.time_game_start = fplGetTimeInMilliseconds();
  game_state.time_last_frame = game_state.time_game_start;

  game_state.quit_game = false;
  game_state.game_initialised = false;

  game_state.storage_permanent.size = megabytes(256);
  game_state.storage_permanent.base = mmap(NULL,
                                           game_state.storage_permanent.size,
                                           PROT_READ | PROT_WRITE,
                                           MAP_PRIVATE | MAP_ANONYMOUS,
                                           -1,
                                           0);
  game_state.storage_permanent.used = 0;

  game_state.storage_transient.size = megabytes(256);
  game_state.storage_transient.base = mmap(NULL,
                                           game_state.storage_transient.size,
                                           PROT_READ | PROT_WRITE,
                                           MAP_PRIVATE | MAP_ANONYMOUS,
                                           -1,
                                           0);
  game_state.storage_transient.used = 0;

  cr_plugin ctx;
  ctx.userdata = &game_state;

  const char *plugin = CR_DEPLOY_PATH "/" CR_PLUGIN(CR_NAME);
  if (access(plugin, F_OK) != -1) {
    RONA_INFO("plugin found at: %s\n", plugin);
    cr_plugin_open(ctx, plugin);
  } else if (access(CR_PLUGIN(CR_NAME), F_OK) != -1) {
    RONA_INFO("plugin found at: %s\n", CR_PLUGIN(CR_NAME));
    cr_plugin_open(ctx, CR_PLUGIN(CR_NAME));
  } else {
    RONA_ERROR("unable to find shared library: %s\n", CR_NAME);
    exit(EXIT_FAILURE);
  }

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

  // disable events, use polling only
  settings.input.disabledEvents = 1;

  if (fplPlatformInit(fplInitFlags_Video, &settings)) {
    LoadGLFunctions(&(game_state.gl));

    fplWindowSize window_size;
    fplGetWindowSize(&window_size);
    game_state.window_width = window_size.width;
    game_state.window_height = window_size.height;
    RONA_INFO("Window Initial size: (%d, %d)\n", window_size.width, window_size.height);

    game_state.input.idx = 1;

    // Event/Main loop
    while (fplWindowUpdate() && !game_state.quit_game) {
      game_state.window_resized = false;

      u64 time = fplGetTimeInMilliseconds();
      game_state.time_delta = time - game_state.time_last_frame;
      game_state.time_last_frame = time;

      // Poll events
      fplEvent ev;
      while (fplPollEvent(&ev)) {
        if (ev.type == fplEventType_Window) {
          switch(ev.window.type) {
          case fplWindowEventType_None:            RONA_INFO("Window Event: None\n"); break;
          case fplWindowEventType_Resized:
            fplWindowSize window_size;
            fplGetWindowSize(&window_size);
            game_state.window_resized = true;
            game_state.window_width = window_size.width;
            game_state.window_height = window_size.height;
            RONA_INFO("Window Event: Resized (%d, %d)\n", window_size.width, window_size.height);
            break;
          case fplWindowEventType_GotFocus:        RONA_INFO("Window Event: GotFocus\n"); break;
          case fplWindowEventType_LostFocus:       RONA_INFO("Window Event: LostFocus\n"); break;
          case fplWindowEventType_Minimized:       RONA_INFO("Window Event: Minimized\n"); break;
          case fplWindowEventType_Maximized:       RONA_INFO("Window Event: Maximized\n"); break;
          case fplWindowEventType_Restored:        RONA_INFO("Window Event: Restored\n"); break;
          case fplWindowEventType_DroppedFiles:    RONA_INFO("Window Event: DroppedFiles\n"); break;
          case fplWindowEventType_Exposed:         RONA_INFO("Window Event: Exposed\n"); break;
          case fplWindowEventType_PositionChanged: RONA_INFO("Window Event: PositionChanged\n"); break;
          case fplWindowEventType_Closed:          RONA_INFO("Window Event: Closed\n"); break;
          case fplWindowEventType_Shown:           RONA_INFO("Window Event: Shown\n"); break;
          case fplWindowEventType_Hidden:          RONA_INFO("Window Event: Hidden\n");  break;
          }
          fflush(stdout);
        }
      }

      fplKeyboardState keyboardState;
      if (fplPollKeyboardState(&keyboardState)) {
        game_state.input.idx = 1 - game_state.input.idx;
        int idx = game_state.input.idx;
        game_state.input.key[idx][Key_Space] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_Space];
        game_state.input.key[idx][Key_Return] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_Return];
        game_state.input.key[idx][Key_Shift] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_Shift];
        game_state.input.key[idx][Key_Control] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_Control];
        game_state.input.key[idx][Key_Escape] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_Escape];
        game_state.input.key[idx][Key_Up] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_Up];
        game_state.input.key[idx][Key_Down] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_Down];
        game_state.input.key[idx][Key_Left] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_Left];
        game_state.input.key[idx][Key_Right] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_Right];
        game_state.input.key[idx][Key_0] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_0];
        game_state.input.key[idx][Key_1] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_1];
        game_state.input.key[idx][Key_2] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_2];
        game_state.input.key[idx][Key_3] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_3];
        game_state.input.key[idx][Key_4] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_4];
        game_state.input.key[idx][Key_5] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_5];
        game_state.input.key[idx][Key_6] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_6];
        game_state.input.key[idx][Key_7] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_7];
        game_state.input.key[idx][Key_8] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_8];
        game_state.input.key[idx][Key_9] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_9];
        game_state.input.key[idx][Key_A] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_A];
        game_state.input.key[idx][Key_B] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_B];
        game_state.input.key[idx][Key_C] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_C];
        game_state.input.key[idx][Key_D] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_D];
        game_state.input.key[idx][Key_E] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_E];
        game_state.input.key[idx][Key_F] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_F];
        game_state.input.key[idx][Key_G] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_G];
        game_state.input.key[idx][Key_H] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_H];
        game_state.input.key[idx][Key_I] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_I];
        game_state.input.key[idx][Key_J] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_J];
        game_state.input.key[idx][Key_K] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_K];
        game_state.input.key[idx][Key_L] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_L];
        game_state.input.key[idx][Key_M] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_M];
        game_state.input.key[idx][Key_N] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_N];
        game_state.input.key[idx][Key_O] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_O];
        game_state.input.key[idx][Key_P] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_P];
        game_state.input.key[idx][Key_Q] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_Q];
        game_state.input.key[idx][Key_R] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_R];
        game_state.input.key[idx][Key_S] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_S];
        game_state.input.key[idx][Key_T] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_T];
        game_state.input.key[idx][Key_U] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_U];
        game_state.input.key[idx][Key_V] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_V];
        game_state.input.key[idx][Key_W] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_W];
        game_state.input.key[idx][Key_X] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_X];
        game_state.input.key[idx][Key_Y] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_Y];
        game_state.input.key[idx][Key_Z] = (RonaButtonState)keyboardState.buttonStatesMapped[fplKey_Z];
      }

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
