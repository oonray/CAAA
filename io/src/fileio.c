#include "fileio.h"

ioStream *NewioStream(int fd,int fd_t,size_t buf_t){
		ioStream *out = calloc(1,sizeof(ioStream));
		check(out!=NULL,"Could not create IO Stream");

		out->fd = fd;
		out->fd_t = fd_t;	
	
		switch(fd_t){
			case SOCKFD:
				out->reader.sockReader = &recv;
				out->writer.sockWriter = &send;
				socket = true;
			break;
			default:
				out->reader.fileReader = &read;
				out->reader.fileWriter = &write;
			break;
		}

		out->in = RingBuffer_New(buf_t);
		check(out->in !=NULL,"Could not create in stream");

		out->out = RingBuffer_New(buf_t);
		check(out->in !=NULL,"Could not create out stream");

		return out;
error:
		return NULL;
}

ioStream *NewioStreamFile(bstring path,int flags,int buf_t){
	return NewioStream(open(bdata(path),flags),FILEDFD,1024*4);
}

ioStream *NewioStreamSocket(int proto,int type,int buf_t){
	return NewioStream(socket(proto,type),SOCKFD,1024*4);
}

void DestroyioStream(ioStream *io){
	if(io != NULL){
		    close(io->fd);
			RingBuffer_Destroy(io->in);
			RingBuffer_Destroy(io->out);
			free(io);
		}
}

int ioRead(ioStream *str,FILE *f, size_t amount){
	int rc = 0;
}

int ioWrite(ioStream *str,FILE *f, size_t amount){

}
