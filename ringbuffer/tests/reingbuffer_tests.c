#include "munit.h"
#include "ringbuffer.h"

static void *tests_setup(const MunitParameter params[],void *user_data){
	RingBuffer *buff = RingBuffer_New(128);
	check(buff != NULL,"Could not create ringbuffer");
	return buff;
error:
	return NULL;
}

static void test_teardown(void *fixture){
	free(fixture);
}

MunitResult test_new(const MunitParameter params[], void* user_data_or_fixture){
	check(user_data_or_fixture != NULL,"Could not create ringbuffer");
	return MUNIT_OK;
error:
	return MUNIT_FAIL;
}

MunitResult test_write(const MunitParameter params[], void* user_data_or_fixture){
	RingBuffer *b = (RingBuffer *)user_data_or_fixture;
	bstring data = cstr2bstr("Hello World!");

	RingBuffer_Write(b,bdata(data),blength(data));
	log_info("%s",RingBuffer_Starts_At(b));

	return MUNIT_OK;
error:
	return MUNIT_FAIL;
}

MunitResult test_read(const MunitParameter params[], void* user_data_or_fixture){
	RingBuffer *b = (RingBuffer *)user_data_or_fixture;
	return MUNIT_OK;
error:
	return MUNIT_FAIL;
}

int main(int argc, char *const *argv){
	MunitTest tests[] = {
		{" test_new",test_new,tests_setup,test_teardown,MUNIT_TEST_OPTION_NONE,NULL},
		{" test_write",test_write,tests_setup,test_teardown,MUNIT_TEST_OPTION_NONE,NULL},
		{NULL,NULL,NULL,NULL,MUNIT_TEST_OPTION_NONE,NULL}
	};
	
	const MunitSuite suite ={
		"Ringbuffer Tests",
		tests,
		NULL,
		1,
		MUNIT_SUITE_OPTION_NONE
	};

	return munit_suite_main(&suite, NULL,0,NULL);
}

