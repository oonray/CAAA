#include "ca_ringbuffer.h"
#include <munit/munit.h>

#ifndef _CA_FILEIO_H
#include <ca_fileio.h>
#endif

#ifndef _CA_SERIAL_H_
#include <ca_serial.h>
#endif

struct tagbstring file = bsStatic("./file.out");
struct tagbstring content = bsStatic("loremipsumsetdoloramet");

MunitResult test_new_file(const MunitParameter params[],
                          void *user_data_or_fixture) {
  ca_io_stream *stream = ca_io_stream_new_file(&file, O_RDWR | 0666, 1024 * 10);
  check(stream != NULL, "Could not create Stream %s", bdata(&file));
  ca_io_stream_destroy(stream);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_new_socket(const MunitParameter params[],
                            void *user_data_or_fixture) {
  ca_io_stream *stream =
      ca_io_stream_new_socket(AF_INET, SOCK_STREAM, 1024 * 10);
  check(stream != NULL, "Could not create Stream");
  ca_io_stream_destroy(stream);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_new_std(const MunitParameter params[],
                         void *user_data_or_fixture) {
  ca_io_stream *stream = ca_io_stream_new(CA_ERR, CA_FILEFD, 1024 * 10);
  check(stream != NULL, "Could not create Stream stderr");
  ca_io_stream_destroy(stream);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_file_read_write(const MunitParameter params[],
                                 void *user_data_or_fixture) {
  ca_io_stream *stream = ca_io_stream_new_file(&file, O_RDWR, 1024 * 10);
  check(stream != NULL, "Could not create Stream %s", bdata(&file));

  int rc = ca_io_stream_buff_write(stream, &content);
  check(rc > 0, "Could not write to buffer");

  rc = 0;
  rc = ca_io_stream_io_write(stream);
  check(rc > 0, "Could not write to io");
  ca_io_stream_destroy(stream);

  stream = ca_io_stream_new_file(&file, O_RDWR, 1024 * 10);
  check(stream != NULL, "Could not open file %s", bdata(&file));

  rc = ca_io_stream_io_read(stream);
  check(rc > 0, "Could not read from io");

  bstring data = ca_io_stream_buff_read(stream);
  check(bstrcmp(data, &content), "Data in file not correct");

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_pipe_new(const MunitParameter params[],
                          void *user_data_or_fixture) {
  ca_io_stream_pipe *p = ca_io_stream_new_pipe(0);
  check(p != NULL, "could not create pipe");
  ca_io_stream_destroy_pipe(p);

  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_pipe_read_write(const MunitParameter params[],
                                 void *user_data_or_fixture) {

  int child_pid;

  ca_io_stream_pipe *p = ca_io_stream_new_pipe(0);
  check(p != NULL, "could not create pipe");

  if ((child_pid = fork()) == -1) {
    munit_error("could not fork");
    goto error;
  }

  bstring in = bfromcstr("Hello Pipe");
  if (child_pid == 0) {
    // child
    int rdy = ca_io_stream_buff_write_pipe(p, CA_OUT, in);
    check(rdy > 0, "no data readdy to write");

    int write = ca_io_stream_io_write_pipe(p, CA_OUT);
    check(write > 0, "did not write");
    ca_io_stream_pipe_close(p, CA_OUT);
    exit(0);
  } else {
    // parent
    int read = ca_io_stream_io_read_pipe(p, CA_INN);
    check(read > 0, "did not read");

    bstring out = ca_io_stream_buff_read_pipe(p, CA_INN);
    ca_io_stream_pipe_close(p, CA_INN);

    check(strcmp(bdata(out), bdata(in)) == 0, "wrong message recieved");
  }

  ca_io_stream_destroy_pipe(p);
  return MUNIT_OK;
error:
  return MUNIT_FAIL;
}

MunitResult test_serial_read_write(const MunitParameter params[],
                                   void *user_data_or_fixture) {

  ca_io_stream *p = ca_io_stream_new_serial(bfromcstr("/dev/cu.usbmodem111201"),
                                            115200, 0, 0, 0);

  // ca_io_stream_buff_write(p, bfromcstr("AT\n\r"));
  // ca_io_stream_io_write(p);

  ca_io_stream_buff_write(p, bfromcstr("AT+CREG=\n\r"));
  ca_io_stream_io_write(p);

  ca_io_stream_io_read(p);
  bstring res = ca_io_stream_buff_read(p);

  log_info("%s", bdata(res));

  ca_io_stream_destroy(p);
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
      //     {" test_file_read_write", test_file_read_write, NULL, NULL,
      //    MUNIT_TEST_OPTION_NONE, NULL},
      {" test_pipe_new", test_pipe_new, NULL, NULL, MUNIT_TEST_OPTION_NONE,
       NULL},
      {" test_pipe_read_write", test_pipe_read_write, NULL, NULL,
       MUNIT_TEST_OPTION_NONE, NULL},
      //{" test_serial_read_write", test_serial_read_write, NULL, NULL,
      // MUNIT_TEST_OPTION_NONE, NULL},
      {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

  const MunitSuite suite = {"IO Tests", tests, NULL, 1,
                            MUNIT_SUITE_OPTION_NONE};

  return munit_suite_main(&suite, NULL, 0, NULL);
}
