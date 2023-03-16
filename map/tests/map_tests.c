#include <ca_map.h>
#include <munit/munit.h>

typedef struct testdata {
  int traverse_called;
  struct tagbstring data1;
  struct tagbstring data2;
  struct tagbstring data3;
  struct tagbstring expect1;
  struct tagbstring expect2;
  struct tagbstring expect3;
  ca_map *ca_map;
} testdata;

testdata *testdata_new() {
  testdata *t = calloc(1, sizeof(testdata));
  t->ca_map = NULL;
  t->traverse_called = 0;
  t->data1 = (struct tagbstring)bsStatic("test data 1");
  t->data2 = (struct tagbstring)bsStatic("test data 2");
  t->data3 = (struct tagbstring)bsStatic("test data 3");
  t->expect1 = (struct tagbstring)bsStatic("the value 1");
  t->expect2 = (struct tagbstring)bsStatic("the value 2");
  t->expect3 = (struct tagbstring)bsStatic("the value 3");
  return t;
}

static int traverse_success_func(ca_map_node *node) {
  log_info("key: %s", (char *)node->key);
  return 0;
}

static int traverse_fail_func(ca_map_node *node) {
  log_info("key: %s", (char *)node->key);
  return 1;
}

static void *setup() { return testdata_new(); }

MunitResult test_new(const MunitParameter params[],
                     void *user_data_or_fixture) {
  testdata *dt = user_data_or_fixture;
  dt->ca_map = ca_map_new(NULL, NULL);
  check(dt->ca_map != NULL, "could not create map");
  free(dt->ca_map);
  free(dt);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_destroy(const MunitParameter params[],
                         void *user_data_or_fixture) {
  testdata *dt = user_data_or_fixture;
  dt->ca_map = ca_map_new(NULL, NULL);
  check(dt->ca_map != NULL, "could not create map");

  ca_map_destroy(dt->ca_map);
  free(dt);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_get_set(const MunitParameter params[],
                         void *user_data_or_fixture) {
  testdata *dt = user_data_or_fixture;
  dt->ca_map = ca_map_new(NULL, NULL);

  check(dt->ca_map != NULL, "could not create map");

  int rc = ca_map_set(dt->ca_map, &dt->data1, &dt->expect1);
  check(rc == 0, "failed to set data1");

  rc = ca_map_set(dt->ca_map, &dt->data2, &dt->expect2);
  check(rc == 0, "failed to set data2");

  rc = ca_map_set(dt->ca_map, &dt->data3, &dt->expect3);
  check(rc == 0, "failed to set data3");

  bstring data = ca_map_get(dt->ca_map, &dt->data1);
  check(bstrcmp(data, &dt->expect1) == 0, "got wrong data");

  data = ca_map_get(dt->ca_map, &dt->data2);
  check(bstrcmp(data, &dt->expect2) == 0, "got wrong data");

  data = ca_map_get(dt->ca_map, &dt->data3);
  check(bstrcmp(data, &dt->expect3) == 0, "got wrong data");

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

int main(int argc, char *const *argv) {
  MunitTest tests[] = {
      {" test_new", test_new, setup, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {" test_destroy", test_destroy, setup, NULL, MUNIT_TEST_OPTION_NONE,
       NULL},
      {" test_get_set", test_get_set, setup, NULL, MUNIT_TEST_OPTION_NONE,
       NULL},
      {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

  const MunitSuite suite = {"ca_map_tests", tests, NULL, 1,
                            MUNIT_SUITE_OPTION_NONE};

  return munit_suite_main(&suite, NULL, 0, NULL);
}
