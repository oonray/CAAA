#ifndef CA_FILEIO_SSL_H_
#define CA_FILEIO_SSL_H_

#include <ca_fileio.h>
#include <openssl/ssl3.h>

typedef int (*ca_sock_reader_ssl)(SSL *, void *, int);
typedef int (*ca_sock_writer_ssl)(SSL *, void *, int);

ca_io_stream *ca_io_stream_new_socket_ssl(SSL *ssl, int inet, int type,
                                          int buf_t);

#endif // CA_FILEIO_SSL_H_
