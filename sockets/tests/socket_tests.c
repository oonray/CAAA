#include "munit.h"
#include "soc.h"

#define PORT 32340

MunitResult test_new(const MunitParameter params[],
                     void *user_data_or_fixture) {
  Asoc *srv = Asoc_New(AF_INET, SOCK_STREAM, PORT, bfromcstr("0.0.0.0"), 0);
  check(srv != NULL, "Failed to create server");
  Asoc_Destroy(srv);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_bind(const MunitParameter params[],
                      void *user_data_or_fixture) {
  Asoc *srv = Asoc_New(AF_INET, SOCK_STREAM, PORT, bfromcstr("0.0.0.0"), 0);
  check(srv != NULL, "Failed to create server");

  check(AsocBind(srv) >= 0, "Could not bind to 0.0.0.0:31337");
  Asoc_Destroy(srv);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_listen(const MunitParameter params[],
                        void *user_data_or_fixture) {
  Asoc *srv = Asoc_New(AF_INET, SOCK_STREAM, PORT + 1, bfromcstr("0.0.0.0"), 0);
  check(srv != NULL, "Failed to create server");

  check(AsocBind(srv) >= 0, "Could not bind to 0.0.0.0:31337");
  check(AsocListen(srv, 0) >= 0, "Could not listen on 0.0.0.0:31337");
  Asoc_Destroy(srv);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_accept(const MunitParameter params[],
                        void *user_data_or_fixture) {
  Asoc *srv = Asoc_New(AF_INET, SOCK_STREAM, PORT + 2, bfromcstr("0.0.0.0"), 0);
  check(srv != NULL, "Failed to create server");

  Asoc *client =
      Asoc_New(AF_INET, SOCK_STREAM, PORT + 2, bfromcstr("127.0.0.1"), 0);
  check(client != NULL, "Failed to create client");

  check(AsocBind(srv) >= 0, "Could not bind to 0.0.0.0:31337");
  check(AsocListen(srv, 0) >= 0, "Could not listen on 0.0.0.0:31337");

  log_info("Listening on 0.0.0.0:31337");
  check(AsocConnect(client) >= 0, "could not connect to localhost:31337");

  Asoc *peer = AsocAccept(srv, 0);
  check(peer != NULL, "Did not accept connection");
  log_info("Connection from %s:%d", inet_ntoa(peer->addr.sin_addr),
           peer->addr.sin_port);

  Asoc_Destroy(client);
  Asoc_Destroy(srv);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

int main(int argc, char *argv[]) {
  MunitTest tests[] = {
      {" test_new", test_new, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {" test_bind", test_bind, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {" test_listen", test_listen, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {" test_accept", test_accept, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

  const MunitSuite suite = {"Socket Tests", tests, NULL, 1,
                            MUNIT_SUITE_OPTION_NONE};

  return munit_suite_main(&suite, NULL, 0, NULL);
}
