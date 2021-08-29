#include "munit.h"
#include "queue.h"

int print_traverse(void *d){
	log_info("%s",(char *)d);
	return 0;
}

void * setup(const MunitParameter params[], void* user_data_or_fixture){
	return Queue_New(sizeof(void *),3);
}

void teardown(void* user_data_or_fixture){
	Queue_Destroy((Queue *)user_data_or_fixture);
}

MunitResult test_new(const MunitParameter params[], void* user_data_or_fixture){
		Queue *st = Queue_New(sizeof(void *),3);	
		check(st != NULL,"Queue Was not Created!");

		return MUNIT_OK;
error:
		return MUNIT_FAIL;
}

MunitResult test_push_pop(const MunitParameter params[], void* user_data_or_fixture){
	Queue *st = (Queue *)user_data_or_fixture;

	char *test[] = {
		"Test1",
		"Test2",
		"Test3"
	};

	for(int i = 0;i<3;i++){
		Queue_Send(st,test[i]);
		check(Queue_Peek(st) == test[0],"Wrong Value Peeked");
	}	

	check(Queue_Count(st) == 3,"Wrong Size");	
	Queue_Traverse(st,print_traverse);

	for(int i=0;i<3;i++){
		char *val = Queue_Recieve(st);
		log_info("%s",val);
		check(val == test[i],"Wrong Value popped, expected %s",test[i]);
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
		"Queue Tests",
		tests,
		NULL,
		1,
		MUNIT_SUITE_OPTION_NONE
	};

	return munit_suite_main(&suite, NULL,0,NULL);
}

