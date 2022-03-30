#include "munit.h"
#include "ssl.h"

#undef CONFIG_FOLDER
#define CONFIG_FOLDER "./s_tests/"

MunitResult test_new_config(const MunitParameter params[],
                            void *user_data_or_fixture) {
  log_info("Reading Config");
  AsocSSLConfig *conf = AsocSSLConfig_New(bfromcstr(CONFIG_FOLDER));
  check(conf != NULL, "Could not create config");
  log_info("Config Read");

  check(bstrcmp(conf->pki, bfromcstr("<path>.pem")) == 0,
        "PKI has wrong value: %s", bdata(conf->pki));
  check(bstrcmp(conf->cert, bfromcstr("<path>.pem")) == 0,
        "Cert has wrong value: %s", bdata(conf->cert));

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

#undef CONFIG_FOLDER
#define CONFIG_FOLDER "../sockets/tests/"

MunitResult test_new_config_conf(const MunitParameter params[],
                                 void *user_data_or_fixture) {
  log_info("Reading Config");
  log_info("%s", CONFIG_FOLDER);
  AsocSSLConfig *conf = AsocSSLConfig_New(bfromcstr(CONFIG_FOLDER));
  check(conf != NULL, "Could not create config");
  log_info("Config Read");

  check(bstrcmp(conf->pki, bfromcstr("../sockets/tests/certs/ssl.key")) == 0,
        "PKI has wrong value: %s", bdata(conf->pki));
  check(bstrcmp(conf->cert, bfromcstr("../sockets/tests/certs/ssl.pem")) == 0,
        "Cert has wrong value: %s", bdata(conf->cert));

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_new(const MunitParameter params[],
                     void *user_data_or_fixture) {
  AsocSSL *srv =
      AsocSSL_New(AF_INET, SOCK_STREAM, 31337, bfromcstr("0.0.0.0"), SERVER);
  check(srv != NULL, "Failed to create server");
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_bind(const MunitParameter params[],
                      void *user_data_or_fixture) {
  AsocSSL *srv =
      AsocSSL_New(AF_INET, SOCK_STREAM, 31337, bfromcstr("0.0.0.0"), SERVER);
  check(srv != NULL, "Failed to create server");

  check(AsocBind(srv->as) >= 0, "Could not bind to 0.0.0.0:31337");
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_listen(const MunitParameter params[],
                        void *user_data_or_fixture) {
  AsocSSL *srv =
      AsocSSL_New(AF_INET, SOCK_STREAM, 31337, bfromcstr("0.0.0.0"), SERVER);

  check(srv != NULL, "Failed to create server");

  check(AsocBind(srv->as) >= 0, "Could not bind to 0.0.0.0:31337");
  check(AsocListen(srv->as, 0) >= 0, "Could not listen on 0.0.0.0:31337");
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_accept(const MunitParameter params[],
                        void *user_data_or_fixture) {
  AsocSSL *srv =
      AsocSSL_New(AF_INET, SOCK_STREAM, 31339, bfromcstr("0.0.0.0"), SERVER);
  check(srv != NULL, "Failed to create server");

  AsocSSL *client =
      AsocSSL_New(AF_INET, SOCK_STREAM, 31339, bfromcstr("0.0.0.0"), CLIENT);
  check(client != NULL, "Failed to create client");

  check(AsocBind(srv->as) >= 0, "Could not bind to 0.0.0.0:31339");
  check(AsocListen(srv->as, 0) >= 0, "Could not listen on 0.0.0.0:31339");

  log_info("Listening on 0.0.0.0:31339");

  int rc = fork();
  if (rc == 0)
    system("openssl s_client -connect 127.0.0.1:31339 -servername localhost");

  Asoc *peer = AsocAccept(srv->as);
  AsocSSL_doSSL(srv);

  check(peer != NULL, "Did not accept connection");
  log_info("Connection from %s:%d", inet_ntoa(peer->addr.sin_addr),
           peer->addr.sin_port);

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_send_recieve(const MunitParameter params[],
                              void *user_data_or_fixture) {
  AsocSSL *srv =
      AsocSSL_New(AF_INET, SOCK_STREAM, 31338, bfromcstr("0.0.0.0"), SERVER);
  check(srv != NULL, "Failed to create server");

  AsocSSL *client =
      AsocSSL_New(AF_INET, SOCK_STREAM, 31338, bfromcstr("0.0.0.0"), CLIENT);
  check(client != NULL, "Failed to create client");

  check(AsocBind(srv->as) >= 0, "Could not bind to 0.0.0.0:31338");
  check(AsocListen(srv->as, 0) >= 0, "Could not listen on 0.0.0.0:31338");

  log_info("Listening on 0.0.0.0:31338");

  int rc = fork();
  if (rc == 0) {
    check(AsocSSLConnect(client) >= 0, "could not connect to localhost:31338");
  }

  Asoc *peer = AsocAccept(srv->as);
  check(peer != NULL, "Did not accept connection");
  log_info("Connection from %s:%d", inet_ntoa(peer->addr.sin_addr),
           peer->addr.sin_port);

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

int main(int argc, char *argv[]) {
  MunitTest tests[] = {
      {" test_new_config", test_new_config, NULL, NULL, MUNIT_TEST_OPTION_NONE,
       NULL},
      {" test_new_config", test_new_config_conf, NULL, NULL,
       MUNIT_TEST_OPTION_NONE, NULL},
      {" test_new", test_new, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {" test_bind", test_bind, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {" test_listen", test_listen, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {" test_accept", test_accept, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {" test_send_recieve", test_send_recieve, NULL, NULL,
       MUNIT_TEST_OPTION_NONE, NULL},

      {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

  const MunitSuite suite = {"Socket Tests", tests, NULL, 1,
                            MUNIT_SUITE_OPTION_NONE};

  return munit_suite_main(&suite, NULL, 0, NULL);
}
