#define FPL_IMPLEMENTATION
#define FPL_NO_VIDEO_SOFTWARE
#define FPL_NO_AUDIO
#include <final_platform_layer.h>

#include <GL/gl.h>

#include <sys/mman.h>

#include <stdbool.h>
#define CR_HOST CR_UNSAFE // try to best manage static states
#include "cr.h"

#include "host_data.h"

const char *plugin = CR_DEPLOY_PATH "/" CR_PLUGIN(CR_NAME);
const char *tmp = "/tmp/";

static HostData data;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

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

static void LoadGLExtensions(HostData *hd) {
  hd->glClear = (PFNGLCLEARPROC)GLProcAddress("glClear");
  hd->glViewport = (PFNGLVIEWPORTPROC)GLProcAddress("glViewport");
  hd->glDrawArrays = (PFNGLDRAWARRAYSPROC)GLProcAddress("glDrawArrays");
  hd->glClearColor = (PFNGLCLEARCOLORPROC)GLProcAddress("glClearColor");
  hd->glGetIntegerv = (PFNGLGETINTEGERVPROC)GLProcAddress("glGetIntegerv");

  hd->glCreateShader = (PFNGLCREATESHADERPROC)GLProcAddress("glCreateShader");
  hd->glShaderSource = (PFNGLSHADERSOURCEPROC)GLProcAddress("glShaderSource");
  hd->glCompileShader = (PFNGLCOMPILESHADERPROC)GLProcAddress("glCompileShader");
  hd->glGetShaderiv = (PFNGLGETSHADERIVPROC)GLProcAddress("glGetShaderiv");
  hd->glAttachShader = (PFNGLATTACHSHADERPROC)GLProcAddress("glAttachShader");
  hd->glCreateProgram = (PFNGLCREATEPROGRAMPROC)GLProcAddress("glCreateProgram");
  hd->glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)GLProcAddress("glGetShaderInfoLog");
  hd->glLinkProgram = (PFNGLLINKPROGRAMPROC)GLProcAddress("glLinkProgram");
  hd->glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)GLProcAddress("glValidateProgram");
  hd->glGetProgramiv = (PFNGLGETPROGRAMIVPROC)GLProcAddress("glGetProgramiv");
  hd->glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)GLProcAddress("glGetProgramInfoLog");
  hd->glDeleteShader = (PFNGLDELETESHADERPROC)GLProcAddress("glDeleteShader");
  hd->glUseProgram = (PFNGLUSEPROGRAMPROC)GLProcAddress("glUseProgram");

  hd->glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)GLProcAddress("glGenVertexArrays");
  hd->glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)GLProcAddress("glBindVertexArray");
  hd->glGenBuffers = (PFNGLGENBUFFERSPROC)GLProcAddress("glGenBuffers");
  hd->glBindBuffer = (PFNGLBINDBUFFERPROC)GLProcAddress("glBindBuffer");
  hd->glBufferData = (PFNGLBUFFERDATAPROC)GLProcAddress("glBufferData");
  hd->glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)GLProcAddress("glEnableVertexAttribArray");
  hd->glDisableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)GLProcAddress("glDisableVertexAttribArray");
  hd->glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)GLProcAddress("glVertexAttribPointer");
  hd->glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)GLProcAddress("glDeleteVertexArrays");
}
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

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
  data.quit_game = false;
  data.game_initialised = false;
  data.screen_width = 800;
  data.screen_height = 450;

  data.mem_size = gigabytes(1);
  data.mem_base = mmap(NULL, data.mem_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  // printf("mem_base %p\n", data.mem_base);
  // *(int *)(data.mem_base) = 42;
  // *((int *)(data.mem_base) + 1) = 12;

  // int *p = (int *)(data.mem_base);
  // printf("%d\n", *p++);
  // printf("%d\n", *p++);

  cr_plugin ctx;
  // the host application should initalize a plugin with a context, a plugin
  // filename without extension and the full path to the plugin
  ctx.userdata = &data;
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

    const char *version = (const char *)glGetString(GL_VERSION);
    const char *vendor = (const char *)glGetString(GL_VENDOR);
    const char *renderer = (const char *)glGetString(GL_RENDERER);
    fplConsoleFormatOut("OpenGL version: %s\n", version);
    fplConsoleFormatOut("OpenGL vendor: %s\n", vendor);
    fplConsoleFormatOut("OpenGL renderer: %s\n", renderer);

    LoadGLExtensions(&data);

		// Event/Main loop
		while (fplWindowUpdate() && !data.quit_game) {
			// Poll events
			fplEvent ev;
			while (fplPollEvent(&ev)) {
				/// ...
			}

      fplWindowSize windowArea;
      fplGetWindowSize(&windowArea);

      data.window_width = windowArea.width;
      data.window_height = windowArea.height;


        // // your code goes here
      // glClearColor(1.0, 0.0, 0.0, 1.0);
      // glClear(GL_COLOR_BUFFER_BIT);

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
