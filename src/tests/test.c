#include "munit/munit.h"
#include <stdlib.h>

#include "../unity_build.h"

static MunitResult test_rona_types(const MunitParameter params[], void* user_data) {
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

static MunitResult test_rona_math(const MunitParameter params[], void* user_data) {
  munit_assert(lerp(1.0f, 0.5f, 10.0f) == 5.5f);
  munit_assert(lerp(10.0f, 0.5f, 1.0f) == 5.5f);

  munit_assert(clamp(10.0f, 12.0f, 15.0f) == 12.0f);
  munit_assert(clamp(10.0f, 9.0f, 15.0f) == 10.0f);
  munit_assert(clamp(10.0f, 16.0f, 15.0f) == 15.0f);

  return MUNIT_OK;
}

static MunitResult test_rona_memory(const MunitParameter params[], void* user_data) {

  BumpAllocator arena;
  arena.size = megabytes(1);
  arena.base = malloc(arena.size);
  arena.used = 0;

  FixedBlockAllocator ma;
  fixed_block_allocator_reset(&ma, &arena, false, NULL);

  void* ptr = rona_malloc(&ma, 500);
  munit_assert(ma.available_one_kilobyte == NULL);

  rona_free(&ma, ptr);

  // added memory block to available list
  munit_assert(ma.available_one_kilobyte != NULL);

  void* ptr2 = rona_malloc(&ma, 500);

  // reusing previous memory block
  munit_assert(ptr2 == ptr);
  munit_assert(ma.available_one_kilobyte == NULL);

  return MUNIT_OK;
}

static MunitResult test_rona_stretchy(const MunitParameter params[], void* user_data) {
  BumpAllocator arena;
  arena.size = megabytes(1);
  arena.base = malloc(arena.size);
  arena.used = 0;

  FixedBlockAllocator ma;
  fixed_block_allocator_reset(&ma, &arena, false, NULL);

  // push existing values onto stretchy buffer
  {
    Vec2i* arr = NULL;

    sb_push(&ma, arr, vec2i(10, 10));
    sb_push(&ma, arr, vec2i(20, 10));
    sb_push(&ma, arr, vec2i(30, 10));
    sb_push(&ma, arr, vec2i(40, 10));
    sb_push(&ma, arr, vec2i(50, 10));

    munit_assert(arr[0].x == 10);
    munit_assert(arr[1].x == 20);
    munit_assert(arr[2].x == 30);
    munit_assert(arr[3].x == 40);
    munit_assert(arr[4].x == 50);

    munit_assert(sb_count(arr) == 5);
  }

  // grow the stretchy buffer before filling in the values
  {
    Vec2i* arr = NULL;
    sb_add(&ma, arr, 2);
    arr[0].x = 5;
    arr[0].y = 5;
    arr[1].x = 8;
    arr[1].y = 8;
    munit_assert(sb_count(arr) == 2);

    sb_add(&ma, arr, 2);
    arr[2].x = 1;
    arr[2].y = 2;
    arr[3].x = 3;
    arr[3].y = 4;
    munit_assert(sb_count(arr) == 4);

    munit_assert(arr[0].x == 5);
    munit_assert(arr[1].x == 8);
    munit_assert(arr[2].x == 1);
    munit_assert(arr[3].x == 3);

    Vec2i last = sb_last(arr);
    munit_assert(last.x == 3);
    munit_assert(last.y == 4);
  }

  return MUNIT_OK;
}

static MunitTest test_suite_tests[] = {
    {(char*)"/rona/types", test_rona_types, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {(char*)"/rona/math", test_rona_math, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {(char*)"/rona/memory", test_rona_memory, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {(char*)"/rona/stretchy", test_rona_stretchy, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite test_suite = {(char*)"", test_suite_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
  return munit_suite_main(&test_suite, (void*)"rona", argc, argv);
}
