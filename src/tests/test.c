#include "munit/munit.h"

#include <stdlib.h>

// float.h for FLT_MAX
#include <float.h>
#include <math.h>

#include "string.h"
#include "stdio.h"

#include "../platform.h"
#include "../rona.h"

#include "../colour.h"
#include "../entity.h"
#include "../game.h"
#include "../input.h"
#include "../level.h"
#include "../level1.h"
#include "../memory_arena.h"
#include "../mesh_block.h"
#include "../mesh_pit.h"
#include "../mesh_hero.h"
#include "../renderer.h"
#include "../rona_math.h"

#include "../colour.c"
#include "../game.c"
#include "../input.c"
#include "../level1.c"
#include "../level.c"
#include "../memory_arena.c"
#include "../mesh_block.c"
#include "../mesh_pit.c"
#include "../mesh_hero.c"
#include "../renderer.c"
#include "../rona_math.c"


static MunitResult
test_rona_types(const MunitParameter params[], void* user_data) {
  munit_assert(sizeof(f32) == 4);
  munit_assert(sizeof(f64) == 8);
  munit_assert(sizeof(u8) == 1);
  munit_assert(sizeof(u16) == 2);
  munit_assert(sizeof(i16) == 2);
  munit_assert(sizeof(u32) == 4);
  munit_assert(sizeof(i32) == 4);
  munit_assert(sizeof(u64) == 8);

  return MUNIT_OK;
}

static MunitTest test_suite_tests[] = {
  { (char*) "/rona/types", test_rona_types, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite test_suite = {
  (char*) "",
  test_suite_tests,
  NULL,
  1,
  MUNIT_SUITE_OPTION_NONE
};



int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
  return munit_suite_main(&test_suite, (void*) "rona", argc, argv);
}
