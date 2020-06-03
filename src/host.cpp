#define FPL_IMPLEMENTATION
#include <final_platform_layer.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <sys/mman.h>

#include <stdbool.h>
#define CR_HOST CR_UNSAFE // try to best manage static states
#include "cr.h"

#include "host_data.h"

const char *plugin = CR_DEPLOY_PATH "/" CR_PLUGIN(CR_NAME);
const char *tmp = "/tmp/";

static HostData data;

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
  fplCopyString("cg2", settings.window.title, fplArrayCount(settings.window.title));

	// Overwrite the video driver
	settings.video.driver = fplVideoDriverType_OpenGL;

	// Modern OpenGL
	settings.video.graphics.opengl.compabilityFlags = fplOpenGLCompabilityFlags_Core;
	settings.video.graphics.opengl.majorVersion = 3;
	settings.video.graphics.opengl.minorVersion = 3;

	if (fplPlatformInit(fplInitFlags_Video, &settings)) {

		// Event/Main loop
		while (fplWindowUpdate() && !data.quit_game) {
			// Poll events
			fplEvent ev;
			while (fplPollEvent(&ev)) {
				/// ...
			}

			// your code goes here
      glClearColor(1.0, 0.0, 0.0, 1.0);
      glClear(GL_COLOR_BUFFER_BIT);

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
