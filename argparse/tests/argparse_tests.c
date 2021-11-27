#include "argparse.h"
#include "munit.h"

static const struct tagbstring description =
    bsStatic("Test Data Argument Parser");

int argc = 1;
char *args[2] = {
    "testparser",
    "-i",
    "5",
};

ArgumentParser *argparser;

static void *setup() { argparser = Argparse_New_Argument_Parser(&description); }

void teardown() {}

MunitResult test_new(const MunitParameter params[],
                     void *user_data_or_fixture) {
  check(user_data_or_fixture != NULL, "Could not create data");

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_add_arg(const MunitParameter params[],
                         void *user_data_or_fixture) {
  check(argparser != NULL, "Could not create data");
  Argparse_Add_Int(argparser, "-i", "data1", "", "The 1 data segment");
  Argparse_Add_Bool(argparser, "-s", "sum", "", "Wether to sum the data");
  Argparse_Add_String(argparser, "-ss", "summing", "no",
                      "Wether to sum the data");

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_parse(const MunitParameter params[],
                       void *user_data_or_fixture) {
  check(argparser != NULL, "Could not create data");
  test_add_arg(params, user_data_or_fixture);
  Argparse_Parse(argparser, argc, args);

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_print_all(const MunitParameter params[],
                           void *user_data_or_fixture) {
  check(argparser->args_t != NULL, "Could not create data");
  check(argparser->args_n != NULL, "Could not create data");
  test_add_arg(params, user_data_or_fixture);
  TriTree_Traverse(argparser->args_n, PrintArgs, NULL);

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

int main(int argc, char *argv[]) {
  MunitTest tests[] = {
      {"test_new", test_new, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
      {"test_add", test_add_arg, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
      {"test_print_all", test_print_all, setup, teardown,
       MUNIT_TEST_OPTION_NONE, NULL},
      {"test_parse", test_parse, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
      {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

  const MunitSuite suite = {"Argparse Tests ", tests, NULL, 1,
                            MUNIT_SUITE_OPTION_NONE};

  return munit_suite_main(&suite, NULL, 0, NULL);
}
