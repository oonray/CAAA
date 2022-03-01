#include "fileio.h"

ioStream *NewIoStream(int fd,int fd_t,size_t buf_t){
		ioStream *out = calloc(1,sizeof(ioStream));
		check(out!=NULL,"Could not create IO Stream");

		out->fd = fd;
		out->fd_t = fd_t;	
	
		switch(fd_t){
			case SOCKFD:
				out->reader.sockReader = &recv;
				out->writer.sockWriter = &send;
			break;
			default:
				out->reader.fileReader = &read;
				out->writer.fileWriter = &write;
			break;
		}

		out->in = RingBuffer_New(buf_t);
		check(out->in !=NULL,"Could not create in stream");

	return out;
error:
		return NULL;
}

ioStream *NewIoStreamFile(bstring path,int flags, int rights, int buf_t){
	const char *pt = bdata(path);
	int fd = open(pt,flags,rights);
	check(fd>=0,"Could not open file");
	return NewIoStream(fd,FILEFD,buf_t);
error:
	return NULL;
}

ioStream *NewIoStreamSocket(int proto,int type,int buf_t){
	int fd = socket(AF_INET,SOCK_STREAM,0);
	check(fd!=0,"Could not open file");
	return NewIoStream(fd,SOCKFD,buf_t);
error:
	return NULL;
}

void DestroyIoStream(ioStream *io){
	if(io != NULL){
		    close(io->fd);
			RingBuffer_Destroy(io->in);
			free(io);
		}
}

int IoStreamIoRead(ioStream *str){
	int rc = 0;
	if(RingBuffer_Avaliable_Data(str->in)==0){
		str->in->start = str->in->end = 0;
	}

	if(str->fd_t == SOCKFD){
		rc = str->reader.sockReader(str->fd,
									 RingBuffer_Starts_At(str->in),
									 RingBuffer_Avaliable_Space(str->in),0);
	}else{
		rc = str->reader.fileReader(str->fd,
									 RingBuffer_Starts_At(str->in),
									 RingBuffer_Avaliable_Space(str->in));
	}

	check(rc!=0,"Failed to read form %s",str->fd_t==SOCKFD?"Socket":"File");
	RingBuffer_Commit_Write(str->in,rc);
	return rc;
error:
	return -1;
}

int IoStreamIoWrite(ioStream *str){
	int rc = 0;
	bstring data = RingBuffer_Get_All(str->in);
	check(data!=NULL,"Failed to get data from buffer");
	check(bfindreplace(data,&NL,&CRLF,0)==BSTR_OK,"Failed to replace new lines");

	if(str->fd_t == SOCKFD){
		rc = str->writer.sockWriter(str->fd,bdata(data),blength(data),0);
	}else{
		rc = str->writer.fileWriter(str->fd,bdata(data),blength(data));
	}

	check(rc==blength(data),"Failed to write to %s",str->fd_t==SOCKFD?"Socket":"File");
	bdestroy(data);

	return rc;
error:
	return -1;
}

bstring IoStreamBuffRead(ioStream *str){
	bstring data = RingBuffer_Get_All(str->in);
	check(data!=NULL,"Failed to get data from buffer");
	return data;
error:
	return NULL;
}

int IoStreamBuffWrite(ioStream *str, bstring input){
	if(RingBuffer_Avaliable_Data(str->in)==0){
		str->in->start = str->in->end = 0;
	}

	int rc = RingBuffer_Write(str->in,bdata(input),blength(input));

	check(rc!=0,"Failed to read form %s",str->fd_t==SOCKFD?"Socket":"File");
	RingBuffer_Commit_Write(str->in,rc);
	return rc;
error:
	return -1;
}
