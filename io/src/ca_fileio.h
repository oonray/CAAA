#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#ifdef CA_FILEIO_SSL_H_
#include <openssl/ssl.h>
#endif
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <bstring/bstrlib.h>
#include <ca_dbg.h>
#include <ca_ringbuffer.h>
#include <ca_serial.h>
#include <ca_vector.h>

// Serial term
#include <termios.h>

#ifndef _CA_FILEIO_H
#define _CA_FILEIO_H

enum ca_socktypes {
  CA_SOCKFD = 0x01,
  CA_FILEFD = 0x03,
  CA_STRINGFD = 0x05,
  CA_PIPEFD = 0x09,
  CA_SSLFD = 0x11,
  CA_SERIAL = 0x13,
};

#define CA_CREATE_RW O_RDWR | O_CREAT
enum ca_descriptors { CA_INN, CA_OUT, CA_ERR };

static struct tagbstring ca_NL = bsStatic("\n");
static struct tagbstring ca_CRLF = bsStatic("\n\r");

// regular open file
typedef size_t (*io_file_reader)(int, void *, size_t);
typedef size_t (*io_file_writer)(int, const void *, size_t);

// socket
typedef ssize_t (*ca_sock_reader)(int, void *, size_t, int);
typedef ssize_t (*ca_sock_writer)(int, const void *, size_t, int);

typedef struct ca_io_stream {
  int fd;
  int fd_t;
  ca_ringbuffer *in;
  void *reader;
  void *writer;
#ifdef CA_FILEIO_SSL_H_
  SSL *ssl;
#endif
  // serial uses tty
  struct termios *tty;
} ca_io_stream;

typedef struct ca_io_stream_pipe {
  int f[2];
  ca_io_stream *in;
  ca_io_stream *out;
} ca_io_stream_pipe;

ca_io_stream *ca_io_stream_new(int fd, int fd_t, size_t buf_t);
ca_io_stream *ca_io_stream_new_file(bstring path, int flags, int buf_t);
ca_io_stream *ca_io_stream_new_socket(int inet, int type, int buf_t);
ca_io_stream_pipe *ca_io_stream_new_pipe(size_t buff_t);

ca_io_stream *ca_io_stream_new_from_file(FILE *fp, int buf_t);
ca_io_stream *ca_io_stream_new_from_soc(int inet, int type, int buf_t,
                                        void *ssl);
ca_io_stream *ca_io_stream_new_serial(bstring path, int baud, int buf_t,
                                      int vtime, int vmin);

void ca_io_stream_destroy(ca_io_stream *io);
void ca_io_stream_destroy_pipe(ca_io_stream_pipe *io);

int ca_io_stream_io_read(ca_io_stream *str);
int ca_io_stream_io_write(ca_io_stream *str);
int ca_io_stream_io_read_pipe(ca_io_stream_pipe *str, int io);
int ca_io_stream_io_write_pipe(ca_io_stream_pipe *str, int io);

bstring ca_io_stream_buff_read(ca_io_stream *str);
int ca_io_stream_buff_write(ca_io_stream *str, bstring input);

bstring ca_io_stream_buff_read_pipe(ca_io_stream_pipe *str, int io);
int ca_io_stream_buff_write_pipe(ca_io_stream_pipe *str, int io, bstring imput);

int ca_io_stream_pipe_close(ca_io_stream_pipe *str, int io);
int ca_io_stream_pipe_open(ca_io_stream_pipe *str, int io);

// File Operations
int ca_io_stream_file(bstring file);
int ca_io_stream_file_create(bstring file, mode_t mode);
int ca_io_stream_file_directory_exists(bstring directory);
int ca_io_stream_file_directory_create(bstring directory, mode_t mode);

#endif
