#include "../src/webserver.h"
#include "dbg_a.h"
#include "munit.h"
#include "request.h"
#include <pthread.h>
#include <unistd.h>

MunitResult test_new(const MunitParameter params[],
                     void *user_data_or_fixture) {
  Webserver *srv = Webserver_New(HTTP, NULL, 0, NULL, NULL);
  check(srv != NULL, "Could not Create Server");

  Webserver_Destroy(srv);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

Webserver *srv;

Response *handle_home(Request *req) {
  Response *rsp =
      Response_New(req, bfromcstr("<pre>Hello World</pre>"), 0, NULL);
  check(rsp != NULL, "Error creating response");
  return rsp;
error:
  return NULL;
}

Response *handle_kill(Request *req) {
  srv->terminate = true;
  Response *rsp = Response_New(
      req, bformat("{\"terminate\":%s}", srv->terminate ? "true" : "false"), 0,
      NULL);
  check(rsp != NULL, "Error creating response");
  return rsp;
error:
  return NULL;
}

void *thread01(void *data) {
  check_none(Webserver_Run((Webserver *)data) == 0, "Error with webserver");
}

void *thread02(void *data) { system(bdata((bstring)data)); }

MunitResult test_new_url(const MunitParameter params[],
                         void *user_data_or_fixture) {
  pthread_t server, client, client2;

  int port = 31320;

  srv = Webserver_New(SOCKFD, NULL, port, NULL, NULL);
  check(srv != NULL, "Could not Create Server");
  log_info("Server Created");

  Webserver_AddRoute(srv, bfromcstr("/"), handle_home);
  Webserver_AddRoute(srv, bfromcstr("/kill"), handle_kill);
  log_info("Routes added");

  pthread_create(&server, NULL, thread01, (void *)srv);

  pthread_create(
      &client, NULL, thread02,
      (void *)bformat("curl -ik http://localhost:%d/ --output -", port));

  pthread_create(
      &client2, NULL, thread02,
      (void *)bformat("curl -ik http://localhost:%d/kill --output -", port));

  pthread_join(server, NULL);
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
