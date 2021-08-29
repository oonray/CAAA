#include "munit.h" 
#include "c/dbg.h"
#include "tritree.h"
#include <bstrlib.h>

typedef struct test_data{
  TriTree *tree;
  int traverse;
  bstring value[4];
  bstring test[4];
}test_data;

int trav = 0;

static void *setup(const MunitParameter params[],void *fixture){ 
  test_data *data = calloc(1,sizeof(test_data));
  check(data!=0,"could not create data");

  data->traverse = 0;

  data->value[0] = cstr2bstr("ValueA");
  data->value[1] = cstr2bstr("ValueB");
  data->value[2] = cstr2bstr("Value2");
  data->value[3] = cstr2bstr("Value4");

  data->test[0] = cstr2bstr("Test");
  data->test[1] = cstr2bstr("Test2");
  data->test[2] = cstr2bstr("Tse");
  data->test[3] = cstr2bstr("T");

  for(int i = 0;i<=3;i++){
    data->tree = TriTree_Insert(data->tree,bdata(data->test[i]),blength(data->test[i]),data->value[i]);
  }

  check(data->tree != NULL,"could not create TriTree");
  return data;
error:
  return NULL;
}


static void teardown(void *fixture){
   test_data *test =(test_data *)fixture; 
   TriTree_Destroy(test->tree);
  for(int i = 0;i<4;i++){
   bdestroy(test->value[i]);
  }

  for(int i = 0;i<4;i++){
   bdestroy(test->test[i]);
  }

   free(test);
}

void traverse(void *value,void *data){
    assert(value != NULL);
    trav++;
}

MunitResult Insert(const MunitParameter params[],void *fixture){
    check(fixture != NULL,"Could not create tritree");
    return MUNIT_OK;
error:
    return MUNIT_FAIL;
}

MunitResult TestSearch(const MunitParameter params[],void *fixture){
    check(fixture != NULL,"Could not create tritree");
    test_data *data = (test_data *) fixture;  
    for(int i = 0;i<4;i++){
      void *res = TriTree_Search(data->tree,bdata(data->test[i]),blength(data->test[i]));
      check(bstrcmp((bstring)res,data->value[i]),"Wrong value recieved for %d",i);
    }
    return MUNIT_OK;
error:
    return MUNIT_FAIL;
}

MunitResult TestSearchPrefix(const MunitParameter params[],void *fixture){
    check(fixture != NULL,"Could not create tritree");
    test_data *data = (test_data *) fixture;  

    void *res = TriTree_Search_Prefix(data->tree,bdata(data->test[0]),blength(data->test[0])); 
    log_info("expected %s, got %s",bdata(data->value[0]),bdata((bstring)res));
    check(res == data->value[0],"expected different value for 0"); 

    res = TriTree_Search_Prefix(data->tree,bdata(data->test[0]),1); 
    log_info("expected %s, got %s",bdata(data->value[0]),bdata((bstring)res));
    check(res == data->value[3],"expected different value for 0"); 

    res = TriTree_Search_Prefix(data->tree,"TE",strlen("TE")); 
    check(res != NULL,"expected parchial"); 

    res = TriTree_Search_Prefix(data->tree,"TE-",strlen("TE-")); 
    check(res != NULL,"expected parchial"); 

    return MUNIT_OK;
error:
    return MUNIT_FAIL;
}

MunitResult TestTraverse(const MunitParameter params[],void *fixture){
    check(fixture != NULL,"Could not create tritree");
    test_data *data = (test_data *) fixture;  
    trav = 0;
    TriTree_Traverse(data->tree,traverse,data);
    check(trav == 4,"did not find 4 keys");
    return MUNIT_OK;
error:
    return MUNIT_FAIL;
}

MunitTest tests[]={
  {"Test Insert",Insert,setup,teardown,MUNIT_TEST_OPTION_NONE,NULL},
  {"Test Search",TestSearch,setup,teardown,MUNIT_TEST_OPTION_NONE,NULL},
  {"Test Search prefix",TestSearch,setup,teardown,MUNIT_TEST_OPTION_NONE,NULL},
  {"Test Traverse",TestTraverse,setup,teardown,MUNIT_TEST_OPTION_NONE,NULL},
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite suite = {
  "TriTree tests ",
  tests,
  NULL,
  1,
  MUNIT_SUITE_OPTION_NONE
};

int main(int argc,char *argv[]){
  return munit_suite_main(&suite,NULL,argc,argv);
}
