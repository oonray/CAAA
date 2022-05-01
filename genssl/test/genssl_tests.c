#include "dbg.h"
#include "genssl.h"
#include "munit.h"
#include "ssl.h"

MunitResult test_new(const MunitParameter params[],
                     void *user_data_or_fixture) {

  log_info("Creating Config");
  AsocSSLConfig *conf = AsocSSLConfig_New_B(bfromcstr("./pki_test/"));
  check(conf != NULL, "Could not allocate config");

  log_info("Creating keypair");
  KeyPair *pair = KeyPair_New(conf);
  check(pair != NULL, "Could not create key pair");

  log_info("%s", bdata(pair->conf->folder));
  log_info("%s",
           bdata(bformat("%s%s", bdata(pair->conf->folder), "public.pem")));
  log_info("Writing KeyPair");
  KeyPair_Write(pair);

  check(IoFileStream_FileExists(bfromcstr("./pki_test/public.pem")) == 0,
        "Key not created");
  check(IoFileStream_FileExists(bfromcstr("./pki_test/private.pem")) == 0,
        "Key not created");

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_self_signed(const MunitParameter params[],
                             void *user_data_or_fixture) {

  log_info("Creating Config");
  AsocSSLConfig *conf = AsocSSLConfig_New_B(bfromcstr("./pki_test/"));
  check(conf != NULL, "Could not allocate config");

  log_info("Creating keypair");
  KeyPair *pair = KeyPair_New(conf);
  check(pair != NULL, "Could not create key pair");

  check(IoFileStream_FileExists(bfromcstr("./pki_test/public.pem")) == 0,
        "Key not created");
  check(IoFileStream_FileExists(bfromcstr("./pki_test/private.pem")) == 0,
        "Key not created");
  check(KeyPair_Read(pair) == 0, "Could not read");

  log_info("Creating Cert");
  X509_Self_Signed *cert = SelfSigned_New(bfromcstr("NO"), bfromcstr("LocalCO"),
                                          bfromcstr("localhost"), 0, pair);
  check(cert != NULL, "Could not create cert");

  log_info("Writing Cert To %s", bdata(cert->pair->conf->folder));
  check(SelfSigned_Write(cert) >= 0, "Could not write cert");

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

int main(int argc, char *argv[]) {
  MunitTest tests[] = {
      {" test_new", test_new, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {" test_self_signed", test_self_signed, NULL, NULL,
       MUNIT_TEST_OPTION_NONE, NULL},
      {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

  const MunitSuite suite = {"Genssl Tests", tests, NULL, 1,
                            MUNIT_SUITE_OPTION_NONE};

  return munit_suite_main(&suite, NULL, 0, NULL);
}
