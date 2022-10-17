#include "munit.h"
#include "list.h"

MunitResult test_new(const MunitParameter params[], void* user_data_or_fixture){
	    List *vec = List_New();
		check(vec != NULL,"Could not create vector")
		return MUNIT_OK;
error:
		return MUNIT_FAIL;
}

MunitResult test_push(const MunitParameter params[], void* user_data_or_fixture){
		char *data[] = {
			"Data1",
			"Data2",
			"Data3"
			};

	    List *vec = List_New();
		check(vec != NULL,"Could not create vector")

		List_Push(vec,sizeof(char *),data[0]);
		List_Push(vec,sizeof(char *),data[1]);
		List_Push(vec,sizeof(char *),data[2]);

		check(vec->length==3,"Wrong length 3 got: %d",vec->length);

		List_Element *data1 = List_Get(vec,0);
		check(strcmp(data1->data,data[0])==0,"Wrong value data1 got: %s",data1->data);

		List_Element *data2 = List_Get(vec,1);
		check(strcmp(data2->data,data[1])==0,"Wrong value data2 got: %s",data2->data);

		List_Element *data3 = List_Get(vec,2);
		check(strcmp(data3->data,data[2])==0,"Wrong value data3 got: %s",data3->data);
		return MUNIT_OK;
error:
		return MUNIT_FAIL;
}

int main(int argc, char *argv[]){
	MunitTest tests[] = {
		{" test_new",test_new,NULL,NULL,MUNIT_TEST_OPTION_NONE,NULL},
		{" test_push",test_push,NULL,NULL,MUNIT_TEST_OPTION_NONE,NULL},
		{NULL,NULL,NULL,NULL,MUNIT_TEST_OPTION_NONE,NULL}
	};
	
	const MunitSuite suite ={
		"List Tests",
		tests,
		NULL,
		1,
		MUNIT_SUITE_OPTION_NONE
	};

	return munit_suite_main(&suite, NULL,0,NULL);
}

