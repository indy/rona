#include "munit/munit.h"
#include "../rona.h"

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

#include <stdlib.h>

int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
  return munit_suite_main(&test_suite, (void*) "rona", argc, argv);
}
