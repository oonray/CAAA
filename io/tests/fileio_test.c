#include "munit.h"

#ifndef _FILEIO_H
#include "fileio.h"
#endif

struct tagbstring file = bsStatic("./file.out");
struct tagbstring content = bsStatic("loremipsumsetdoloramet");

MunitResult test_new_file(const MunitParameter params[],
                          void *user_data_or_fixture) {
  ioStream *stream = NewIoStreamFile(&file, CREATE_RW | 0766, 1024 * 10);
  check(stream != NULL, "Could not create Stream %s", bdata(&file));
  DestroyIoStream(stream);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_new_socket(const MunitParameter params[],
                            void *user_data_or_fixture) {
  ioStream *stream = NewIoStreamSocket(AF_INET, SOCK_STREAM, SOCKFD, 1024 * 10);
  check(stream != NULL, "Could not create Stream");
  DestroyIoStream(stream);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_new_std(const MunitParameter params[],
                         void *user_data_or_fixture) {
  ioStream *stream = NewIoStream(ERR, FILEFD, 1024 * 10);
  check(stream != NULL, "Could not create Stream stderr");
  DestroyIoStream(stream);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_file_read_write(const MunitParameter params[],
                                 void *user_data_or_fixture) {
  ioStream *stream = NewIoStreamFile(&file, O_RDWR, 1024 * 10);
  check(stream != NULL, "Could not create Stream %s", bdata(&file));

  int rc = IoStreamBuffWrite(stream, &content);
  check(rc > 0, "Could not write to buffer");

  rc = 0;
  rc = IoStreamIoWrite(stream);
  check(rc > 0, "Could not write to io");
  DestroyIoStream(stream);

  stream = NewIoStreamFile(&file, O_RDWR, 1024 * 10);
  check(stream != NULL, "Could not open file %s", bdata(&file));

  rc = IoStreamIoRead(stream);
  check(rc > 0, "Could not read from io");

  bstring data = IoStreamBuffRead(stream);
  check(bstrcmp(data, &content), "Data in file not correct");

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

int main(int argc, char *argv[]) {
  MunitTest tests[] = {
      {" test_new_file", test_new_file, NULL, NULL, MUNIT_TEST_OPTION_NONE,
       NULL},
      {" test_new_sock", test_new_socket, NULL, NULL, MUNIT_TEST_OPTION_NONE,
       NULL},
      {" test_new_std", test_new_std, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {" test_file_read_write", test_file_read_write, NULL, NULL,
       MUNIT_TEST_OPTION_NONE, NULL},
      {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

  const MunitSuite suite = {"IO Tests", tests, NULL, 1,
                            MUNIT_SUITE_OPTION_NONE};

  return munit_suite_main(&suite, NULL, 0, NULL);
}
