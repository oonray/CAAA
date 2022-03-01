#include <stdlib.h>
#include <sys/select.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#include <c/dbg.h>
#include <c/ringbuffer.h>
#include <c/vector.h>
#include <c/bstring/bstrlib.h>

#ifndef _FILEIO_H
#define _FILEIO_H

#define SOCKFD 0x01
#define FILEFD 0x03

enum Descriptors{INN,OUT,ERR};

static struct tagbstring NL = bsStatic("\n");
static struct tagbstring CRLF = bsStatic("\n\r");

typedef union ioReader{
	ssize_t (*fileReader)(int,void *, size_t);
	ssize_t (*sockReader)(int,void *, size_t,int);
}ioReader;

typedef union ioWriter{
	ssize_t (*fileWriter)(int,const void *, size_t);
	ssize_t (*sockWriter)(int,const void *, size_t,int);
}ioWriter;

typedef struct ioStream {
		int fd;
		int fd_t;
		RingBuffer *in;
		ioReader reader;
		ioWriter writer;
} ioStream;


ioStream *NewIoStream(int fd,int fd_t,size_t buf_t);
ioStream *NewIoStreamFile(bstring path,int flags,int rights,int buf_t);
ioStream *NewIoStreamSocket(int proto,int type,int buf_t);

void DestroyIoStream(ioStream *io);

int IoStreamIoRead(ioStream *str);
int IoStreamIoWrite(ioStream *str);

bstring IoStreamBuffRead(ioStream *str);
int IoStreamBuffWrite(ioStream *str, bstring input);

#endif
