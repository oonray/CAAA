#include "bstrlib.h"
#include "dbg.h"
#include "munit.h"
#include "ringbuffer.h"
//#include <curl/base64.h>

#ifndef XOR_H_
#include "xor.h"
#endif

const char *plain = "Hello World";
const char xored[] = {0x2e, 0x3,  0xa, 0x9, 0x46, 0x31,
                      0x9,  0x14, 0xa, 0x2, 0x0};

MunitResult test_new_key(const MunitParameter params[],
                         void *user_data_or_fixture) {

  ioStream *key = xor_default_key(sizeof(char) * 10);
  check_memory(key);

  bstring data = IoStreamBuffRead(key);
  check(blength(data) == sizeof(char) * 10, "Data should be 10 characters");

  log_info("\n%s", bdata(data));
  bdestroy(data);

  return MUNIT_OK;
error:
  DestroyIoStream(key);
  return MUNIT_FAIL;
}

MunitResult test_encrypt(const MunitParameter params[],
                         void *user_data_or_fixture) {

  XOR * xor = XORNew(NULL);
  ioStream *in = NewIoStream(0, STRINGFD, 0);
  IoStreamBuffWrite(in, bfromcstr(plain));

  ioStream *out = XOR_encrypt(xor, in);
  check(out != NULL, "Out cannot be null");

  check(RingBuffer_Avaliable_Data(out->in) > 0, "No data given");

  bstring out_s = IoStreamBuffRead(out);

  ioStream *chk = XOR_decrypt(xor, out);
  bstring chk_s = IoStreamBuffRead(out);

  check(bstrcmp(out_s, chk_s),
        "Strings are not the same after decrypt got [%s]", bdata(chk_s));

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

int main(int argc, char *argv[]) {
  MunitTest tests[] = {
      {" test_new_key", test_new_key, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {" test_encrypt", test_encrypt, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

  const MunitSuite suite = {"IO Tests", tests, NULL, 1,
                            MUNIT_SUITE_OPTION_NONE};

  return munit_suite_main(&suite, NULL, 0, NULL);
}
