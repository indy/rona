#define FPL_IMPLEMENTATION
#include <final_platform_layer.h>

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "cr.h"

#include <GL/gl.h>
#include "gfx.h"
#include "game_state.h"

#include "game.h"

static GameState     *g_game_state = 0; // hold user data kept on host and received from host

// To save states automatically from previous instance to a new loaded one, use CR_STATE flag on statics/globals.
// This will create a new data section in the binary for transferable states between instances that will be copied
// and kept around to overwrite the new instance initial states. That means that things that were initialized in
// a previous loaded states will continue from where they were.
static unsigned int CR_STATE version = 1;
// But unfortunately new added states in new instances (post the fact states) do not work because they will be
// overwritten by previous states (in this case, inexisting ones being zeroed), so uncommenting the following line
// will have a value of zero.
//static int32_t CR_STATE sad_state = 2;
// For this reason, cr is better for runtime modifing existing stuff than adding/removing new stuff.
// At the sime time, if we remove a state variable, the reload will safely fail with the error CR_STATE_INVALIDATED.
// A rollback will be effectued and this error can be dealt client side, for exemple, by poping a dialog box asking
// to force reload cleaning up states (restarting the client from scratch with the new version).

CR_EXPORT int cr_main(struct cr_plugin *ctx, enum cr_op operation) {
  assert(ctx);

  g_game_state = (GameState *)ctx->userdata;

  if (operation == CR_CLOSE) {
    game_shutdown(g_game_state);
    return 0;
  }

  if (operation != CR_STEP) {
    fprintf(stdout, "OP: %s(%d)\n", operation == CR_LOAD ? "LOAD" : "UNLOAD", ctx->version);
    return 0;
  }

  // crash protection may cause the version to decrement. So we can test current version against one
  // tracked between instances with CR_STATE to signal that we're not running the most recent instance.
  if (ctx->version < version) {
    // a failure code is acessible in the `failure` variable from the `cr_plugin` context.
    // on windows this is the structured exception error code, for more info:
    //      https://msdn.microsoft.com/en-us/library/windows/desktop/ms679356(v=vs.85).aspx
    fprintf(stdout, "A rollback happened due to failure: %x!\n", ctx->failure);
  }
  version = ctx->version;

  // Not this does not carry state between instances (no CR_STATE), this means each time we load an instance
  // this value will be reset to its initial state (true), and then we can print the loaded instance version
  // one time only by instance version.
  static bool print_version = true;
  if (print_version) {
    fprintf(stdout, "loaded version: %d\n", ctx->version);

    // disable further printing for this instance only
    print_version = false;
  }

  if (!g_game_state->game_initialised) {
    game_init(g_game_state);
    g_game_state->game_initialised = true;
    return 0;
  }

  game_tick(g_game_state);
  return 0;
}
