#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "bstrlib.h"
#include "dbg.h"
#include "ringbuffer.h"
#include "vector.h"

#ifndef _FILEIO_H
#define _FILEIO_H

#define SOCKFD 0x01
#define FILEFD 0x03
#define SSLFD 0x11

enum Descriptors { INN, OUT, ERR };

static struct tagbstring NL = bsStatic("\n");
static struct tagbstring CRLF = bsStatic("\n\r");

typedef union ioReader {
  ssize_t (*fileReader)(int, void *, size_t);
  ssize_t (*sockReader)(int, void *, size_t, int);
#ifdef HEADER_SSL_H
  ssize_t (*sslSockReader)(struct ssl_st *, void *, size_t, int);
#endif
} ioReader;

typedef union ioWriter {
  ssize_t (*fileWriter)(int, const void *, size_t);
  ssize_t (*sockWriter)(int, const void *, size_t, int);
#ifdef HEADER_SSL_H
  ssize_t (*sslSockWriter)(struct ssl_st *, void *, size_t, int);
#endif
} ioWriter;

typedef struct ioStream {
  int fd;
  int fd_t;
  RingBuffer *in;
  ioReader reader;
  ioWriter writer;
} ioStream;

ioStream *NewIoStream(int fd, int fd_t, size_t buf_t);
ioStream *NewIoStreamFile(bstring path, int flags, int rights, int buf_t);
ioStream *NewIoStreamSocket(int inet, int type, int FD, int buf_t);

ioStream *NewIoStreamSocketSOC(int inet, int type, int buf_t);
#ifdef HEADER_SSL_H
ioStream *NewIoStreamSocketSSL(SSL *ssl, int inet, int type, int buf_t);
#endif

void DestroyIoStream(ioStream *io);

int IoStreamIoRead(ioStream *str);
int IoStreamIoWrite(ioStream *str);

bstring IoStreamBuffRead(ioStream *str);
int IoStreamBuffWrite(ioStream *str, bstring input);

// File Operations
int IoFileStream_FileExists(bstring file);
int IoFileStream_FileCreate(bstring file, int prem);
int IoFileStream_DirectoryExists(bstring directory);
int IoFileStream_DirectoryCreate(bstring directory, int prem);

#endif
