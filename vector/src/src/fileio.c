#include "fileio.h"

ioStream *NewioStream(int fd_t,int buf_t){
		ioStream *out = calloc(1,sizeof(ioStream));
		check(out!=NULL,"Could not create IO Stream");

		out->fd_t = fd_t;	
	
		switch(fd_t){
			case SOCKFD:
				out->reader.sockReader = &recv;
			break;
			case FILEFD: 
			out->reader.fileReader = &read;
			break;
			default:
			break;
		}

		out->in = RingBuffer_New(1024*4);
		out->out = RingBuffer_New(1024*4);

		return out;
error:
		return NULL;
}

void DestroyioStream(ioStream *io){
	if(io != NULL){
			RingBuffer_Destroy(io->in);
			RingBuffer_Destroy(io->out);
			free(io);
		}
}
