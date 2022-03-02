#include "munit.h"
#include "soc.h"

MunitResult test_new(const MunitParameter params[], void* user_data_or_fixture){
        AsocServer *srv = AsocServer_New(AF_INET,SOCK_STREAM);
        check(srv != NULL,"Failed to create server");
		return MUNIT_OK;
error:
		return MUNIT_FAIL;
}

int main(int argc, char *argv[]){
	MunitTest tests[] = {
		{" test_new",test_new,NULL,NULL,MUNIT_TEST_OPTION_NONE,NULL},
		{NULL,NULL,NULL,NULL,MUNIT_TEST_OPTION_NONE,NULL}
	};

	const MunitSuite suite ={
		"Socket Tests",
		tests,
		NULL,
		1,
		MUNIT_SUITE_OPTION_NONE
	};

	return munit_suite_main(&suite, NULL,0,NULL);
}

#include "soc.h"


