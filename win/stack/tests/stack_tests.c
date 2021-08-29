#include "munit.h"
#include "stack.h"

int print_traverse(void *d){
	log_info("%s",(char *)d);
	return 0;
}

void * setup(const MunitParameter params[], void* user_data_or_fixture){
	return Stack_New(sizeof(void *),3);
}

void teardown(void* user_data_or_fixture){
	Stack_Destroy((Stack *)user_data_or_fixture);
}

MunitResult test_new(const MunitParameter params[], void* user_data_or_fixture){
		Stack *st = Stack_New(sizeof(void *),3);	
		check(st != NULL,"Stack Was not Created!");

		return MUNIT_OK;
error:
		return MUNIT_FAIL;
}

MunitResult test_push_pop(const MunitParameter params[], void* user_data_or_fixture){
	Stack *st = (Stack *)user_data_or_fixture;

	char *test[] = {
		"Test1",
		"Test2",
		"Test3"
	};

	for(int i = 0;i<3;i++){
		Stack_Push(st,test[i]);
		check(Stack_Peek(st) == test[i],"Wrong Value Peeked");
	}	

	check(Stack_Count(st) == 3,"Wrong Size");	
	Stack_Traverse(st,print_traverse);

	for(int i=2;i>=0;i--){
		char *val = Stack_Pop(st);
		check(val == test[i],"Wrong Value popped");
	}
	return MUNIT_OK;
error:
	return MUNIT_FAIL;
}

int main(int argc, char *const *argv){
	MunitTest tests[] = {
		{"test_create",test_new,NULL,NULL,MUNIT_TEST_OPTION_NONE,NULL},
		{"test_push_pop",test_push_pop,setup,teardown,MUNIT_TEST_OPTION_NONE,NULL},
		{NULL,NULL,NULL,NULL,MUNIT_TEST_OPTION_NONE,NULL}
	};
	
	const MunitSuite suite ={
		"Stack Tests",
		tests,
		NULL,
		1,
		MUNIT_SUITE_OPTION_NONE
	};

	return munit_suite_main(&suite, NULL,0,NULL);
}

