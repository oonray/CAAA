#include "dbg.h"
#include "munit.h"

#ifndef XOR_H_
#include "xor.h"
#endif

MunitResult test_new_key(const MunitParameter params[],
                         void *user_data_or_fixture) {

  ioStream *key = xor_default_key(1024);
  check_memory(key);

  bstring data = IoStreamBuffRead(key);

  log_info("%s", bdata(data));

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

int main(int argc, char *argv[]) {
  MunitTest tests[] = {
      {" test_new_key", test_new_key, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

  const MunitSuite suite = {"IO Tests", tests, NULL, 1,
                            MUNIT_SUITE_OPTION_NONE};

  return munit_suite_main(&suite, NULL, 0, NULL);
}
