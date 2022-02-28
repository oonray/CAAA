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

#define SOCKFD 0x01
#define FILEFD 0x03

struct tagbstring NL = bsStatic("\n");
struct tagbstring CRLF = bsStatic("\n\r");

typedef union ioReader{
	ssize_t (*fileReader)(int, void *, size_t); 
	ssize_t (*sockReader)(int, void *, size_t,int);
}ioReader;

typedef union ioWriter{
	ssize_t (*fileWriter)(int, void *, size_t); 
	ssize_t (*sockWriter)(int, void *, size_t,int);
}ioWriter;

typedef struct ioStream {
		int fd;
		int fd_t;
		RingBuffer *in;
		RingBuffer *out;
		ioReader reader;
		ioWriter writer;
} ioStream;


ioStream *NewioStream(int fd,int fd_t,int buf_t);
ioStream *NewioStreamFile(bstring path,int buf_t);
ioStream *NewioStreamSocket(int proto,int type,int buf_t);

void DestroyioStream(ioStream *io);

int ioRead(ioStream *str,FILE *f,size_t amount);
int ioWrite(ioStream *str,FILE *f,size_t amount);

bstring ioReadFile(bstring *filename,int type);
void ioCopy(ioStream a,ioStream b);
