#include "../src/request.h"
#include "map.h"
#include "munit.h"
#include "tritree.h"

void traverse(void *value, void *data) {
  printf("%s\n", bdata((bstring)value));
}

MunitResult test_new(const MunitParameter params[],
                     void *user_data_or_fixture) {
  ioStream *req = NewIoStreamFile(bfromcstr("../webserver/tests/request.req"),
                                  O_RDWR, 0644, 1024 * 10);
  check(IoStreamIoRead(req) > 0, "Could not read request");

  Request *r = Request_New(RingBuffer_Get_All(req->in));
  check(r != NULL, "Error in getting Request");

  log_info("Request using %s V:%s URI: %s", bdata(r->method), bdata(r->version),
           bdata(r->uri));

  TriTree_Traverse(r->Headers, traverse, NULL);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

int main(int argc, char *argv[]) {
  MunitTest tests[] = {
      {" requests tests", test_new, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

  const MunitSuite suite = {"Vector Tests", tests, NULL, 1,
                            MUNIT_SUITE_OPTION_NONE};

  return munit_suite_main(&suite, NULL, 0, NULL);
}
