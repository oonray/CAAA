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
  return Response_New(NULL, NULL, 500, NULL);
  ;
}

Response *handle_kill(Request *req) {
  Response *rsp = Response_New(
      req, bformat("{\"terminate\":%s}", srv->terminate ? "true" : "false"), 0,
      NULL);
  check(rsp != NULL, "Error creating response");
  srv->terminate = true;
  return rsp;
error:
  return Response_New(NULL, NULL, 500, NULL);
}

void *thread01(void *data) {
  int ret = 1;
  Webserver_Run((Webserver *)data);
  return (void *)&ret;
}

void *thread02(void *data) {
  log_info("running %s", bdata((bstring)data));
  system(bdata((bstring)data));
  int ret = 1;
  return (void *)&ret;
}

MunitResult test_new_url(const MunitParameter params[],
                         void *user_data_or_fixture) {
  pthread_t server, client, client2, client3;
  time_t t;

  srand((unsigned)time(&t));
  int port = (rand() % 10) + 31300;

  srv = Webserver_New(SOCKFD, NULL, port, NULL, NULL);
  check(srv != NULL, "Could not Create Server");
  log_info("Server Created");

  Webserver_AddRoute(srv, bfromcstr("/"), handle_home);
  Webserver_AddRoute(srv, bfromcstr("/kill"), handle_kill);
  log_info("Routes added");

  log_info("t01");

  pthread_create(&server, NULL, thread01, (void *)srv);

  sleep(1);

  log_info("t02");
  pthread_create(
      &client, NULL, thread02,
      (void *)bformat("curl -ik http://localhost:%d/ --output -", port));

  pthread_join(client, NULL);

  log_info("t03");
  pthread_create(
      &client3, NULL, thread02,
      (void *)bformat("curl -ik http://localhost:%d/asdfasdfasdf --output -",
                      port));

  pthread_join(client3, NULL);

  log_info("t04");
  pthread_create(
      &client2, NULL, thread02,
      (void *)bformat("curl -ik http://localhost:%d/kill --output -", port));

  pthread_join(server, NULL);
  log_info("join");

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

struct tagbstring conf_folder = bsStatic("../sockets/tests/");
static AsocSSLConfig *conf;

MunitResult test_new_url_ssl(const MunitParameter params[],
                             void *user_data_or_fixture) {
  pthread_t server, client, client2;
  time_t t;

  srand((unsigned)time(&t));
  int port = (rand() % 10) + 31300;
  conf = AsocSSLConfig_New(&conf_folder);

  srv = Webserver_New(SSLFD, NULL, port, NULL, conf);
  check(srv != NULL, "Could not Create Server");
  log_info("Server Created");

  Webserver_AddRoute(srv, bfromcstr("/"), handle_home);
  Webserver_AddRoute(srv, bfromcstr("/kill"), handle_kill);
  log_info("Routes added");

  log_info("t01");
  pthread_create(&server, NULL, thread01, (void *)srv);

  sleep(2);
  log_info("t02");
  pthread_create(
      &client, NULL, thread02,
      (void *)bformat("curl -i -k https://localhost:%d/ --output -", port));

  pthread_join(client, NULL);

  sleep(2);
  log_info("t02");
  pthread_create(
      &client, NULL, thread02,
      (void *)bformat("curl -i -k https://localhost:%d/ --output -", port));

  pthread_join(client, NULL);

  log_info("t03");
  pthread_create(
      &client2, NULL, thread02,
      (void *)bformat("curl -i -k https://localhost:%d/kill --output -", port));

  pthread_join(server, NULL);
  log_info("join");
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
      {" webserver test url ssl", test_new_url_ssl, NULL, NULL,
       MUNIT_TEST_OPTION_NONE, NULL},
      {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

  const MunitSuite suite = {"Webserver Tests", tests, NULL, 1,
                            MUNIT_SUITE_OPTION_NONE};

  return munit_suite_main(&suite, NULL, 0, NULL);
}
