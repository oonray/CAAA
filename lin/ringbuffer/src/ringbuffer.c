#include "ringbuffer.h"

RingBuffer *RingBuffer_New(int length){
	RingBuffer *buffer = (RingBuffer *)calloc(1,sizeof(RingBuffer));
	buffer->length = length +1;	
	buffer->start = 0;
	buffer->end = 0;
	buffer->buffer = (char *)calloc(buffer->length,1);
	return buffer;
}

void RingBuffer_Destroy(RingBuffer *r){
	if(r){
		free(r->buffer);
		free(r);
	}
}

int RingBuffer_Write(RingBuffer *r,char *data,int length){
	if(RingBuffer_Avaliable_Data(r) == 0){
		r->start = r->end = 0;
	}
	
	check(length <= RingBuffer_Avaliable_Space(r),"Not Enough Space: %d request, %d avaliable",RingBuffer_Avaliable_Data(r),length);
	void *result = memcpy(RingBuffer_Ends_At(r),data,length);
	check(result != NULL,"Failed To Write Data int buffer");

	RingBuffer_Commit_Write(r,length);
	return length;

error:
	return -1;
}

int RingBuffer_Read(RingBuffer *r,char *target, int amount){
	check(amount <= RingBuffer_Avaliable_Data(r),"Not enough in the buffer: has %d, needs %d",RingBuffer_Avaliable_Data(r),amount);	
	void *result = memcpy(target,RingBuffer_Starts_At(r),amount);
	check(result != NULL,"Failed to write buffer into data.");

	RingBuffer_Commit_Read(r,amount);
	if(r->end == r->start){
		r->start = r->end = 0;
	}

	return amount;
error:
	return -1;
}

bstring RingBuffer_Gets(RingBuffer *r, int amount){
	check(amount > 0,"Need more than 0 for gets, you gave: %d",amount);
	check(amount <= RingBuffer_Avaliable_Data(r),"Not enough in the buffer.");
	bstring result = blk2bstr(RingBuffer_Starts_At(r),amount);

	check(result != NULL,"Failed to create result");
	check(blength(result) == amount,"Wrong length result");


	RingBuffer_Commit_Read(r,amount);
	check(RingBuffer_Avaliable_Data(r)>=0,"Error in read commit");

	return result;
error:
	return NULL;
}
