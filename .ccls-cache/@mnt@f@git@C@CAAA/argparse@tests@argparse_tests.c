#include "munit.h"
#include "argparse.h"

const struct tagbstring description = bsStatic("Test Data Argument Parser");
const struct tagbstring args[] = {
    bsStatic("testparser"),
    bsStatic("-h"),
};

static void *setup(){
	char *larg[2] = {bdata(&args[0]),bdata(&args[1])};
	return (void *)Argparse_New_Argument_Parser((bstring)&description, 2, larg);
}

void teardown(){}

MunitResult test_new(const MunitParameter params[], void* user_data_or_fixture){
	    check(user_data_or_fixture!=NULL,"Could not create data");

		ArgumentParser *arg = (ArgumentParser *)user_data_or_fixture;
		check(bstrcmp((bstring)Vector_Get(arg->args_v,0),&args[0])==0,"The Value of arg0 is wrong");
		check(bstrcmp((bstring)Vector_Get(arg->args_v,1),&args[1])==0,"The Value of arg1 is wrong");

		Argparse_Parse(arg);
		Argparse_Print_Help(arg);
		return MUNIT_OK;
error:
		return MUNIT_FAIL;
}

int main(int argc, char *argv[]){
	MunitTest tests[] = {
		{"test_new",test_new,setup,teardown,MUNIT_TEST_OPTION_NONE,NULL},
		{NULL,NULL,NULL,NULL,MUNIT_TEST_OPTION_NONE,NULL}
	};
	
	const MunitSuite suite ={
		"Argparse Tests ",
		tests,
		NULL,
		1,
		MUNIT_SUITE_OPTION_NONE
	};

	return munit_suite_main(&suite, NULL,0,NULL);
}

