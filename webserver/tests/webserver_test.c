#include "../src/webserver.h"
#include "munit.h"
#include "request.h"

MunitResult test_new(const MunitParameter params[],
                     void *user_data_or_fixture) {
  Webserver *srv = Webserver_New(HTTP, NULL, 0);
  check(srv != NULL, "Could not Create Server");

  Webserver_Destroy(srv);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

Response *handle_home(Request *req) {
  Response *rsp =
      Response_New(req, bfromcstr("<pre>Hello World</pre>"), 0, NULL);
  check(rsp != NULL, "Error creating response");
  return rsp;
error:
  return NULL;
}

MunitResult test_new_url(const MunitParameter params[],
                         void *user_data_or_fixture) {
  Webserver *srv = Webserver_New(SOCKFD, NULL, 31337);
  check(srv != NULL, "Could not Create Server");

  Webserver_AddRoute(srv, bfromcstr("/"), handle_home);
  Webserver_Run(srv);

  Webserver_Destroy(srv);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

int main(int argc, char *argv[]) {
  MunitTest tests[] = {
      {" webserver tests", test_new, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {" webserver test url", test_new_url, NULL, NULL, MUNIT_TEST_OPTION_NONE,
       NULL},
      {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

  const MunitSuite suite = {"Webserver Tests", tests, NULL, 1,
                            MUNIT_SUITE_OPTION_NONE};

  return munit_suite_main(&suite, NULL, 0, NULL);
}
