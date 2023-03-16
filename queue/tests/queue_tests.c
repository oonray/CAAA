#include <ca_queue.h>
#include <munit/munit.h>

int print_traverse(void *d) {
  log_info("%s", (char *)d);
  return 0;
}

void *setup(const MunitParameter params[], void *user_data_or_fixture) {
  return ca_queue_new(sizeof(void *), 3);
}

void teardown(void *user_data_or_fixture) {
  ca_queue_destroy((ca_queue *)user_data_or_fixture);
}

MunitResult test_new(const MunitParameter params[],
                     void *user_data_or_fixture) {
  ca_queue *st = ca_queue_new(sizeof(void *), 3);
  check(st != NULL, "ca_queue was not created!");

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_push_pop(const MunitParameter params[],
                          void *user_data_or_fixture) {
  ca_queue *st = (ca_queue *)user_data_or_fixture;

  char *test[] = {"Test1", "Test2", "Test3"};

  for (int i = 0; i < 3; i++) {
    ca_queue_send(st, test[i]);
    check(ca_queue_peek(st) == test[0], "wrong value peeked");
  }

  check(ca_queue_count(st) == 3, "wrong size");
  ca_queue_traverse(st, print_traverse);

  for (int i = 0; i < 3; i++) {
    char *val = ca_queue_recieve(st);
    log_info("%s", val);
    check(val == test[i], "wrong value popped, expected %s", test[i]);
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

  const MunitSuite suite = {"ca_queue Tests", tests, NULL, 1,
                            MUNIT_SUITE_OPTION_NONE};

  return munit_suite_main(&suite, NULL, 0, NULL);
}
