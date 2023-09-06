#include <ca_ringbuffer.h>
#include <munit/munit.h>

static void *tests_setup(const MunitParameter params[], void *user_data) {
  ca_ringbuffer *buff = ca_ringbuffer_new(128);
  check(buff != NULL, "could not create ringbuffer");
  return buff;
error:
  return NULL;
}

static void test_teardown(void *fixture) { free(fixture); }

MunitResult test_new(const MunitParameter params[],
                     void *user_data_or_fixture) {
  check(user_data_or_fixture != NULL, "Could not create ringbuffer");
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_write(const MunitParameter params[],
                       void *user_data_or_fixture) {
  ca_ringbuffer *b = (ca_ringbuffer *)user_data_or_fixture;
  bstring data = cstr2bstr("hello world!");

  ca_ringbuffer_write(b, bdata(data), blength(data));

  bstring out = ca_ringbuffer_get_all(b);
  log_info("%s", bdata(out));

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_read(const MunitParameter params[],
                      void *user_data_or_fixture) {
  ca_ringbuffer *b = (ca_ringbuffer *)user_data_or_fixture;
  bstring data = cstr2bstr("hello world!");

  ca_ringbuffer_write(b, bdata(data), blength(data));

  bstring out = ca_ringbuffer_get_all(b);
  log_info("%s", bdata(out));

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

int main(int argc, char *const *argv) {
  MunitTest tests[] = {{" test_new", test_new, tests_setup, test_teardown,
                        MUNIT_TEST_OPTION_NONE, NULL},
                       {" test_write", test_write, tests_setup, test_teardown,
                        MUNIT_TEST_OPTION_NONE, NULL},
                       {" test_read", test_read, tests_setup, test_teardown,
                        MUNIT_TEST_OPTION_NONE, NULL},
                       {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

  const MunitSuite suite = {"Ringbuffer Tests", tests, NULL, 1,
                            MUNIT_SUITE_OPTION_NONE};

  return munit_suite_main(&suite, NULL, 0, NULL);
}
