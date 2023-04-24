#include <ca_fileio_ssl.h>

ca_io_stream *ca_io_stream_new_socket_ssl(SSL *ssl, int inet, int type,
                                          int buf_t) {
  int s_fd = socket(inet, type, 0);
  check(s_fd != 0, "Could not open file");

  ca_io_stream *str = ca_io_stream_new(s_fd, CA_SSLFD, buf_t);
  check(str != NULL, "Could not create stream");

  str->ssl = ssl;
  return str;
error:
  return NULL;
}
