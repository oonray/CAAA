#include <ca_bintree.h>
#include <munit/munit.h>

static struct tagbstring key1 = bsStatic("Key 1");
static struct tagbstring key2 = bsStatic("Key 2");
static struct tagbstring key3 = bsStatic("Key 3");

static struct tagbstring exp1 = bsStatic("1");
static struct tagbstring exp2 = bsStatic("2");
static struct tagbstring exp3 = bsStatic("3");

ca_bintree *tree;
int trav = 0;

static void *setup(const MunitParameter params[], void *fixture) {
  tree = ca_bintree_create(NULL);
  check(tree != NULL, "could not create data");

  return tree;
error:
  return NULL;
}

static int traverse_good(ca_bintreenode *node) {
  log_info("Key: %s\n", bdata((bstring)node->key));
  trav++;
  return 0;
}

static void teardown(void *fixture) {
  ca_bintree_destroy(tree);
  bdestroy(&key1);
  bdestroy(&key2);
  bdestroy(&key3);
  bdestroy(&exp1);
  bdestroy(&exp2);
  bdestroy(&exp3);
}

MunitResult Create(const MunitParameter params[], void *fixture) {
  check(tree != NULL, "Could not create bintree");
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult getset(const MunitParameter params[], void *fixture) {
  check(tree != NULL, "Could not access bintree");

  int rc = ca_bintree_set(tree, &key1, &exp1);
  check(rc == 0, "Could not set %s", bdata(&key1));

  bstring result = ca_bintree_get(tree, &key1);
  check(bstrcmp(result, &exp1) == 0, "found %s, exptected %s", bdata(result),
        bdata(&exp1));
  bdestroy(result);

  rc = ca_bintree_set(tree, &key2, &exp2);
  check(rc == 0, "Could not set %s", bdata(&key2));
  result = ca_bintree_get(tree, &key2);
  check(bstrcmp(result, &exp2) == 0, "found %s, exptected %s", bdata(result),
        bdata(&exp2));
  bdestroy(result);

  rc = ca_bintree_set(tree, &key3, &exp3);
  check(rc == 0, "Could not set %s", bdata(&key3));
  result = ca_bintree_get(tree, &key3);
  check(bstrcmp(result, &exp3) == 0, "found %s, exptected %s", bdata(result),
        bdata(&exp3));

  bdestroy(result);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult traverse_test(const MunitParameter params[], void *fixture) {
  getset(params, fixture);

  int result = ca_bintree_traverse(tree, traverse_good);
  check(trav == 3, "The result is wrong expected 3, got %d", trav);

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult delete (const MunitParameter params[], void *fixture) {
  getset(params, fixture);

  bstring deleted = (bstring)ca_bintree_delete(tree, &key1);
  check(deleted != NULL, "Got NULL on delete");
  check(deleted == &exp1, "Got wrong value on delete expected %s got %s",
        bdata(deleted), bdata(&exp1));

  bstring result = ca_bintree_get(tree, &key1);
  check(result == NULL, "Should get NULL on delete");

  deleted = (bstring)ca_bintree_delete(tree, &key2);
  check(deleted != NULL, "Got NULL on delete");
  check(deleted == &exp2, "Got wrong value on delete expected %s got %s",
        bdata(deleted), bdata(&exp2));

  result = ca_bintree_get(tree, &key2);
  check(result == NULL, "Should get NULL on delete");

  deleted = (bstring)ca_bintree_delete(tree, &key3);
  check(deleted != NULL, "Got NULL on delete");
  check(deleted == &exp3, "Got wrong value on delete expected %s got %s",
        bdata(deleted), bdata(&exp3));

  result = ca_bintree_get(tree, &key3);
  check(result == NULL, "Should get NULL on delete");

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitTest tests[] = {
    {"Test Create", Create, setup, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"Test Get Set", getset, setup, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"Test Traverse", traverse_test, setup, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"Test Delete", delete, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"bintree tests", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char *argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
