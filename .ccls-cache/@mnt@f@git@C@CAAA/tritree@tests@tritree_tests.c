#include "../src/dbg.h"
#include "../src/tritree.h"
#include "bstrlib.h"
#include "munit.h"

TriTree *tree;

static struct tagbstring value0 = bsStatic("ValueA");
static struct tagbstring value1 = bsStatic("ValueB");
static struct tagbstring value2 = bsStatic("Value2");
static struct tagbstring value3 = bsStatic("Value4");

static struct tagbstring key0 = bsStatic("Key");
static struct tagbstring key1 = bsStatic("Key2");
static struct tagbstring key2 = bsStatic("Tse");
static struct tagbstring key3 = bsStatic("T");

int trav = 0;

static void *setup(const MunitParameter params[], void *fixture) {

  tree = TriTree_Insert(tree, bdata(&key0), blength(&key0), &value0);
  check(tree != NULL, "could not create TriTree");

  tree = TriTree_Insert(tree, bdata(&key1), blength(&key1), &value1);
  check(tree != NULL, "could not create TriTree");

  tree = TriTree_Insert(tree, bdata(&key2), blength(&key2), &value2);
  check(tree != NULL, "could not create TriTree");

  tree = TriTree_Insert(tree, bdata(&key3), blength(&key3), &value3);
  check(tree != NULL, "could not create TriTree");

  return tree;
error:
  return NULL;
}

static void teardown(void *fixture) { TriTree_Destroy(tree); }

void traverse(void *value, void *data) {
  assert(value != NULL);
  trav++;
}

MunitResult Insert(const MunitParameter params[], void *fixture) {
  check(fixture != NULL, "Could not create tritree");
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult KeySearch(const MunitParameter params[], void *fixture) {
  check(tree != NULL, "Could not create tritree");

  bstring out = (bstring)TriTree_Search(tree, bdata(&key0), blength(&key0));
  check(bstrcmp(out, &value0) == 0, "Wrong Value Recieved got %s", bdata(out));

  out = (bstring)TriTree_Search(tree, bdata(&key1), blength(&key1));
  check(bstrcmp(out, &value1) == 0, "Wrong Value Recieved");

  out = (bstring)TriTree_Search(tree, bdata(&key2), blength(&key2));
  check(bstrcmp(out, &value2) == 0, "Wrong Value Recieved");

  out = (bstring)TriTree_Search(tree, bdata(&key3), blength(&key3));
  check(bstrcmp(out, &value3) == 0, "Wrong Value Recieved");
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

/*
MunitResult KeySearchPrefix(const MunitParameter params[], void *fixture) {
  check(tree != NULL, "Could not create tritree");

  bstring res =
      (bstring)TriTree_Search_Prefix(tree, bdata(key0), blength(key0));
  log_info("expected %s, got %s", bdata(value0), bdata((bstring)res));
  check(bstrcmp(res, &value0) == 0, "expected different value for 0");

  res = (bstring)TriTree_Search_Prefix(tree, bdata(key3), 1);
  log_info("expected %s, got %s", bdata(value3), bdata((bstring)res));
  check(bstrcmp(res, &value3) == 0, "expected different value for 0");

  res = (bstring)TriTree_Search_Prefix(tree, "TE", strlen("TE"));
  check(res != NULL, "expected parchial");

  res = (bstring)TriTree_Search_Prefix(tree, "TE-", strlen("TE-"));
  check(res != NULL, "expected parchial");

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}
*/

MunitResult KeyTraverse(const MunitParameter params[], void *fixture) {
  check(tree != NULL, "Could not create tritree");
  trav = 0;

  TriTree_Traverse(tree, traverse, NULL);
  check(trav == 4, "did not find 4 keys");

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitTest tests[] = {
    {"Key Insert", Insert, setup, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"Key Search", KeySearch, setup, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    //{"Key Search prefix", KeySearch, setup, NULL, MUNIT_TEST_OPTION_NONE,
    //NULL},
    {"Key Traverse", KeyTraverse, setup, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"TriTree keys ", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char *argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
