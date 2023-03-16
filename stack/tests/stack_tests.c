#include <ca_stack.h>
#include <munit/munit.h>

int print_traverse(void *d) {
  log_info("%s", (char *)d);
  return 0;
}

void *setup(const MunitParameter params[], void *user_data_or_fixture) {
  return ca_stack_new(sizeof(void *), 3);
}

void teardown(void *user_data_or_fixture) {
  ca_stack_destroy((ca_stack *)user_data_or_fixture);
}

MunitResult test_new(const MunitParameter params[],
                     void *user_data_or_fixture) {
  ca_stack *st = ca_stack_new(sizeof(void *), 3);
  check(st != NULL, "ca_stack was not created!");

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_push_pop(const MunitParameter params[],
                          void *user_data_or_fixture) {
  ca_stack *st = (ca_stack *)user_data_or_fixture;

  char *test[] = {"test1", "test2", "test3"};

  for (int i = 0; i < 3; i++) {
    ca_stack_push(st, test[i]);
    check(ca_stack_peek(st) == test[i], "wrong value peeked");
  }

  check(ca_stack_count(st) == 3, "wrong size");
  ca_stack_traverse(st, print_traverse);

  for (int i = 2; i >= 0; i--) {
    char *val = ca_stack_pop(st);
    check(val == test[i], "wrong value popped");
  }
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

int main(int argc, char *const *argv) {
  MunitTest tests[] = {
      {"test_create", test_new, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {"test_push_pop", test_push_pop, setup, teardown, MUNIT_TEST_OPTION_NONE,
       NULL},
      {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

  const MunitSuite suite = {"Ca_Stack Tests", tests, NULL, 1,
                            MUNIT_SUITE_OPTION_NONE};

  return munit_suite_main(&suite, NULL, 0, NULL);
}
