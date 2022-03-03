#include "munit.h"
#include "soc.h"

MunitResult test_new(const MunitParameter params[],
                     void *user_data_or_fixture) {
  AsocServer *srv =
      AsocServer_New(AF_INET, SOCK_STREAM, 31337, bfromcstr("0.0.0.0"));
  check(srv != NULL, "Failed to create server");
  AsocServer_Destroy(srv);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_bind(const MunitParameter params[],
                      void *user_data_or_fixture) {
  AsocServer *srv =
      AsocServer_New(AF_INET, SOCK_STREAM, 31337, bfromcstr("0.0.0.0"));
  check(srv != NULL, "Failed to create server");

  check(AsocBind(srv) >= 0, "Could not bind to 0.0.0.0:31337");
  AsocServer_Destroy(srv);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_listen(const MunitParameter params[],
                        void *user_data_or_fixture) {
  AsocServer *srv =
      AsocServer_New(AF_INET, SOCK_STREAM, 31337, bfromcstr("0.0.0.0"));
  check(srv != NULL, "Failed to create server");

  check(AsocBind(srv) >= 0, "Could not bind to 0.0.0.0:31337");
  check(AsocListen(srv, 0) >= 0, "Could not listen on 0.0.0.0:31337");
  AsocServer_Destroy(srv);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_accept(const MunitParameter params[],
                        void *user_data_or_fixture) {
  AsocServer *srv =
      AsocServer_New(AF_INET, SOCK_STREAM, 31337, bfromcstr("0.0.0.0"));
  check(srv != NULL, "Failed to create server");

  ioStream *client = NewIoStreamSocket(AF_INET, SOCK_STREAM, 1024 * 10);
  check(client != NULL, "Failed to create client");

  check(AsocBind(srv) >= 0, "Could not bind to 0.0.0.0:31337");
  check(AsocListen(srv, 0) >= 0, "Could not listen on 0.0.0.0:31337");

  log_info("Listening on 0.0.0.0:31337");
  check(connect(client->fd, (struct sockaddr *)&srv->addr, sizeof(srv->addr)) >=
            0,
        "could not connect to 0.0.0.0:31337");

  struct sockaddr_in *peer = AsocAccept(srv);
  check(peer != NULL, "Did not accept connection");
  log_info("Connection from %s:%d", inet_ntoa(peer->sin_addr), peer->sin_port);

  DestroyIoStream(client);
  AsocServer_Destroy(srv);
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
