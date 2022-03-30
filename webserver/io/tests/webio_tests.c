#include "munit.h"
#include "request.h"

MunitResult test_new(const MunitParameter params[], void* user_data_or_fixture){
        ioStream *stream = NewIoStreamFile(bfromcstr("../webserver/io/tests/request.req"),O_RDWR,0655,10*1024);
        IoStreamIoRead(stream);

        bstring req_data = RingBuffer_Get_All(stream->in);
        Request *req = Request_New(req_data);

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
		"webio Tests",
		tests,
		NULL,
		1,
		MUNIT_SUITE_OPTION_NONE
	};

	return munit_suite_main(&suite, NULL,0,NULL);
}
