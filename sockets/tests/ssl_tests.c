#include "bstrlib.h"
#include "munit.h"
#include "pthread.h"
#include "ssl.h"

#define PORT 31338

#undef CONFIG_FOLDER
#define CONFIG_FOLDER "s_tests/"

struct tagbstring s_client =
    bsStatic("echo \"exit\" | openssl s_client -connect 127.0.0.1:31337");

struct tagbstring s_server =
    bsStatic("openssl s_server -brief -port 31337 -cert "
             "\"../sockets/tests/certs/ssl.pem\" "
             "-key \"../sockets/tests/certs/ssl.key\"");

void *server_ssl_cert_test(void *param) {
  system(bdata(&s_server));
  return NULL;
}
void *client_ssl_cert_test(void *param) {
  system(bdata(&s_client));
  return NULL;
}

MunitResult test_certs(const MunitParameter params[],
                       void *user_data_or_fixture) {

  pthread_t server, client;
  pthread_create(&server, NULL, server_ssl_cert_test, NULL);
  pthread_create(&client, NULL, client_ssl_cert_test, NULL);
  pthread_join(client, NULL);
  log_info("Client done");
  pthread_cancel(server);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_new_config(const MunitParameter params[],
                            void *user_data_or_fixture) {
  log_info("Reading Config from %s", CONFIG_FOLDER);
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

  log_info("Reading Config from %s", CONFIG_FOLDER);

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
  AsocSSL *srv = AsocSSL_New(AF_INET, SOCK_STREAM, 30000, bfromcstr("0.0.0.0"),
                             SERVER, NULL);
  check(srv != NULL, "Failed to create server");
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_bind(const MunitParameter params[],
                      void *user_data_or_fixture) {
  AsocSSL *srv = AsocSSL_New(AF_INET, SOCK_STREAM, PORT, bfromcstr("0.0.0.0"),
                             SERVER, NULL);
  check(srv != NULL, "Failed to create server");

  check(AsocBind(srv->as) >= 0, "Could not bind to 0.0.0.0:%d", PORT);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_listen(const MunitParameter params[],
                        void *user_data_or_fixture) {
  AsocSSL *srv = AsocSSL_New(AF_INET, SOCK_STREAM, PORT + 1,
                             bfromcstr("0.0.0.0"), SERVER, NULL);

  check(srv != NULL, "Failed to create server");

  check(AsocBind(srv->as) >= 0, "Could not bind to 0.0.0.0:31337");
  check(AsocListen(srv->as, 0) >= 0, "Could not listen on 0.0.0.0:31337");
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

void *thread01(void *data) {
  AsocSSL *srv = (AsocSSL *)data;
  check(srv != NULL, "Asoc is empty");

  check(AsocBind(srv->as) >= 0, "Could not bind to %s:%s", bdata(srv->as->host),
        bdata(srv->as->port));

  check(AsocListen(srv->as, 0) >= 0, "Could not listen on %s:%s",
        bdata(srv->as->host), bdata(srv->as->port));

  log_info("Listening on %s:%s", bdata(srv->as->host), bdata(srv->as->port));
  AsocSSL *peer = AsocSSL_Accept(srv);

  return peer;
error:
  return NULL;
}
void *thread02(void *data) {
  bstring cmd = (bstring)data;
  log_info("Running: %s", bdata(cmd));
  system(bdata(cmd));
  return NULL;
}

MunitResult test_accept(const MunitParameter params[],
                        void *user_data_or_fixture) {

  pthread_t server_t, client_t;
  AsocSSL *srv = AsocSSL_New(AF_INET, SOCK_STREAM, PORT + 1,
                             bfromcstr("0.0.0.0"), SERVER, NULL);

  check(srv != NULL, "Failed to create server");
  log_info("Trying %s:%s", bdata(srv->as->host), bdata(srv->as->port));

  pthread_create(&server_t, NULL, thread01, (void *)srv);
  sleep(1);
  pthread_create(
      &client_t, NULL, thread02,
      (void *)bformat(
          "openssl s_client -connect localhost:%d -servername localhost",
          PORT + 1));

  Asoc *peer = NULL;
  pthread_join(server_t, (void **)&peer);
  check(peer != NULL, "Did not accept connection");
  log_info("Connection from %s:%d", inet_ntoa(peer->addr.sin_addr),
           peer->addr.sin_port);

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_send_recieve(const MunitParameter params[],
                              void *user_data_or_fixture) {
  AsocSSL *srv = AsocSSL_New(AF_INET, SOCK_STREAM, PORT + 2,
                             bfromcstr("0.0.0.0"), SERVER, NULL);
  check(srv != NULL, "Failed to create server");

  AsocSSL *client = AsocSSL_New(AF_INET, SOCK_STREAM, PORT + 2,
                                bfromcstr("0.0.0.0"), CLIENT, NULL);
  check(client != NULL, "Failed to create client");

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

int main(int argc, char *argv[]) {
  MunitTest tests[] = {
      {" test_certs", test_certs, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {" test_new_config", test_new_config, NULL, NULL, MUNIT_TEST_OPTION_NONE,
       NULL},
      {" test_new_config", test_new_config_conf, NULL, NULL,
       MUNIT_TEST_OPTION_NONE, NULL},
      {" test_new", test_new, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {" test_bind", test_bind, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {" test_listen", test_listen, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {" test_accept", test_accept, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      /*     {" test_send_recieve", test_send_recieve, NULL, NULL,
      MUNIT_TEST_OPTION_NONE,
      NULL},*/

      {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

  const MunitSuite suite = {"Socket Tests", tests, NULL, 1,
                            MUNIT_SUITE_OPTION_NONE};

  return munit_suite_main(&suite, NULL, 0, NULL);
}
