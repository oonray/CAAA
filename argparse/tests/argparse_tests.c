#include "argparse.h"
#include "munit.h"

static struct tagbstring description = bsStatic("Test Data Argument Parser");
static struct tagbstring name = bsStatic("test_parser");

int argc = 6;
char *args[6] = {"testparser", "-i", "5", "-s", "-ss", "hello"};

ArgumentParser *argparser;

static void *setup() {
  argparser = Argparse_New_Argument_Parser(&description, &name);
}

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

MunitResult test_get(const MunitParameter params[],
                     void *user_data_or_fixture) {
  check(argparser != NULL, "Could not create data");
  test_add_arg(params, user_data_or_fixture);

  check(Argparse_Parse(argparser, argc, args) == 0, "Could not parse data");
  log_info("Data Parsed");

  Argument *data1 = Argparse_Get(argparser, bfromcstr("data1"));
  check(data1 != NULL, "Could not find data");
  check(bstrcmp(data1->value, bfromcstr("5")), "Value Recieved Wrong: got %s",
        bdata(data1->value));

  Argument *sum = Argparse_Get(argparser, bfromcstr("sum"));
  check(data1 != NULL, "Could not find data");
  check(bstrcmp(sum->value, bfromcstr("true")) == 0,
        "Value Recieved Wrong: got %s", bdata(sum->value));

  Argument *summing = Argparse_Get(argparser, bfromcstr("summing"));
  check(data1 != NULL, "Could not find data");
  check(bstrcmp(summing->value, bfromcstr("no")),
        "Value Recieved Wrong: got %s", bdata(summing->value));

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_print_all(const MunitParameter params[],
                           void *user_data_or_fixture) {
  check(argparser->args_t != NULL, "Could not create data");
  check(argparser->args_n != NULL, "Could not create data");
  test_add_arg(params, user_data_or_fixture);

  Argparse_Print_Help(argparser);

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
      {"test_get", test_get, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
      {"test_parse", test_parse, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
      {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

  MunitSuite suite = {"Argparse Tests ", tests, NULL, 1,
                      MUNIT_SUITE_OPTION_NONE};

  return munit_suite_main(&suite, NULL, 0, NULL);
}
