#include "../src/request.h"
#include "dbg.h"
#include "map.h"
#include "munit.h"
#include "tritree.h"

void traverse(void *value, void *data) {
  printf("%s\n", bdata((bstring)value));
}

MunitResult test_new(const MunitParameter params[],
                     void *user_data_or_fixture) {
  log_info("Reading File");
  ioStream *req = NewIoStreamFile(bfromcstr("../webserver/tests/request.req"),
                                  O_RDWR, 1024 * 10);
  check(IoStreamIoRead(req) > 0, "Could not read request");

  log_info("Loading Request");
  Request *r = Request_New(RingBuffer_Get_All(req->in));
  check(r != NULL, "Error in getting Request");

  log_info("Request using %s V:%s URI: %s", bdata(r->method), bdata(r->version),
           bdata(r->uri));

  bstring host = Map_Get(r->Headers, bfromcstr("Host"));
  check(bstrcmp(host, bfromcstr("localhost")), "got Wrong value %s",
        bdata(host));

  check(bstrcmp(r->body,
                bfromcstr(
                    "This is the boddy of christ.\nAlso this is his blood.")) ==
            0,
        "Got wrong body: \n------------\n%s\n------------\n", bdata(r->body));

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_code(const MunitParameter params[],
                      void *user_data_or_fixture) {
  // 404
  bstring code = Response_Status_TXT(404);
  check(bstrcmp(code, bfromcstr("Not Found")) == 0,
        "Wrong response text recieved");

  // 200
  code = Response_Status_TXT(200);
  check(bstrcmp(code, bfromcstr("OK")) == 0, "Wrong response text recieved");
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_response(const MunitParameter params[],
                          void *user_data_or_fixture) {
  ioStream *req = NewIoStreamFile(bfromcstr("../webserver/tests/request.req"),
                                  O_RDWR, 1024 * 10);
  check(IoStreamIoRead(req) > 0, "Could not read request");

  log_info("Loading Request");
  Request *r = Request_New(RingBuffer_Get_All(req->in));
  check(r != NULL, "Error in getting Request");
  log_info("Request Loaded");

  // Request GOT
  // Default
  log_info("Creating Response");
  Response *rsp = Response_New(r, bfromcstr(""), 0, NULL);
  check(rsp != NULL, "error when creating request");

  log_info("Response Created");
  check(rsp->status_code == 200, "wrong code got when expecting 200");
  check(bstrcmp(rsp->reason_phrase, bfromcstr("OK")) == 0,
        "wrong reason got %s", bdata(rsp->reason_phrase));
  check(bstrcmp(rsp->version, r->version) == 0, "Wrong version returned");
  log_info("%s", bdata(rsp->status_line));

  log_info("\n------\n%s\n------\n", bdata(Response_To_String(rsp)));

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

int main(int argc, char *argv[]) {
  MunitTest tests[] = {
      {" requests tests", test_new, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {" response code test", test_code, NULL, NULL, MUNIT_TEST_OPTION_NONE,
       NULL},

      {" response test", test_response, NULL, NULL, MUNIT_TEST_OPTION_NONE,
       NULL},
      {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

  const MunitSuite suite = {"Request Tests", tests, NULL, 1,
                            MUNIT_SUITE_OPTION_NONE};

  return munit_suite_main(&suite, NULL, 0, NULL);
}
