#include "munit.h" 
#include "bintree.h"

typedef struct test_data{
  BinTree *tree;
  int traverse;
  bstring expect[3];
  bstring key[3];
}test_data;

int trav = 0;

static void *setup(const MunitParameter params[],void *fixture){ 
  test_data *data = calloc(1,sizeof(test_data));
  check(data!=0,"could not create data");

  data->tree = BinTree_Create(NULL);
  data->traverse = 0;

  data->key[0] = bfromcstr("Key 1");
  data->key[1] = bfromcstr("Key 2");
  data->key[2] = bfromcstr("Key 3");

  data->expect[0] = bfromcstr("1");
  data->expect[1] = bfromcstr("2");
  data->expect[2] = bfromcstr("3");

  return data;
error:
  return NULL;
}

static int traverse_good(BinTreeNode *node){
  log_info("Key: %s\n",bdata((bstring)node->key));
  trav++;
  return 0;
}

static void teardown(void *fixture){
   test_data *test =(test_data *)fixture; 
   BinTree_Destroy(test->tree);
   bdestroy(test->expect[0]);
   bdestroy(test->expect[1]);
   bdestroy(test->expect[2]);
   free(test);
}

MunitResult Create(const MunitParameter params[],void *fixture){
    check(fixture != NULL,"Could not create bintree");
    return MUNIT_OK;
error:
    return MUNIT_FAIL;
}

MunitResult getset(const MunitParameter params[],void *fixture){
    check(fixture != NULL,"Could not create bintree");
    test_data *data = (test_data *)fixture;

    int rc = BinTree_Set(data->tree,data->key[0],data->expect[0]);
    check(rc == 0,"Could not set %s",bdata(data->key[0]));
    bstring result = BinTree_Get(data->tree,data->key[0]); 
    check(bstrcmp(result,data->expect[0]) == 0, "found %s, exptected %s",bdata(result),bdata(data->expect[0]));
    bdestroy(result); 

    rc = BinTree_Set(data->tree,data->key[1],data->expect[1]);
    check(rc == 0,"Could not set %s",bdata(data->key[1]));
    result = BinTree_Get(data->tree,data->key[1]); 
    check(bstrcmp(result,data->expect[1]) == 0, "found %s, exptected %s",bdata(result),bdata(data->expect[1]));

    rc = BinTree_Set(data->tree,data->key[2],data->expect[2]);
    check(rc == 0,"Could not set %s",bdata(data->key[2]));
    result = BinTree_Get(data->tree,data->key[2]); 
    check(bstrcmp(result,data->expect[2]) == 0, "found %s, exptected %s",bdata(result),bdata(data->expect[2]));

    bdestroy(result); 
    return MUNIT_OK;
error:
    return MUNIT_FAIL;
}

MunitResult traverse(const MunitParameter params[],void *fixture){
  check(fixture != NULL,"Could not create bintree");
  test_data *data = (test_data *)fixture;
  getset(NULL, data);

  int result = BinTree_Traverse(data->tree,traverse_good);
  check(trav == 3,"The result is wrong expected 3, got %d",result);
  
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult delete(const MunitParameter params[],void *fixture){
  check(fixture != NULL,"Could not create bintree");
  test_data *data = (test_data *)fixture;
  getset(NULL, data);

  bstring deleted = (bstring) BinTree_Delete(data->tree,data->key[0]);
  check(deleted != NULL,"Got null on delete");
  check(deleted==data->expect[0], "Got wrong value on delete expected %s got %s",bdata(deleted),bdata(data->expect[0]));

  bstring result = BinTree_Get(data->tree,data->key[0]);
  check(result == NULL,"Should get null on delete");

  deleted = (bstring) BinTree_Delete(data->tree,data->key[1]);
  check(deleted != NULL,"Got null on delete");
  check(deleted==data->expect[1], "Got wrong value on delete expected %s got %s",bdata(deleted),bdata(data->expect[1]));

  result = BinTree_Get(data->tree,data->key[1]);
  check(result == NULL,"Should get null on delete");

  deleted = (bstring) BinTree_Delete(data->tree,data->key[2]);
  check(deleted != NULL,"Got null on delete");
  check(deleted==data->expect[2], "Got wrong value on delete expected %s got %s",bdata(deleted),bdata(data->expect[2]));

  result = BinTree_Get(data->tree,data->key[2]);
  check(result == NULL,"Should get null on delete");

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitTest tests[]={
  {"Test Create",Create,setup,teardown,MUNIT_TEST_OPTION_NONE,NULL},
  {"Test get set",getset,setup,teardown,MUNIT_TEST_OPTION_NONE,NULL},
  {"Test traverse",traverse,setup,teardown,MUNIT_TEST_OPTION_NONE,NULL},
  {"Test delete",delete,setup,teardown,MUNIT_TEST_OPTION_NONE,NULL},
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite suite = {
  "bintree tests",
  tests,
  NULL,
  1,
  MUNIT_SUITE_OPTION_NONE
};

int main(int argc,char *argv[]){
  return munit_suite_main(&suite,NULL,argc,argv);
}
