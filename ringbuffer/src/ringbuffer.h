#ifndef __ringbuffer_h
#define __ringbuffer_h

#include "bstrlib.h"
#include "dbg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  bstring buffer;
  int length;
  int start;
  int end;
} RingBuffer;

RingBuffer *RingBuffer_New(int length);
void RingBuffer_Destroy(RingBuffer *r);

int RingBuffer_Read(RingBuffer *r, char *target, int amount);
int RingBuffer_Write(RingBuffer *r, char *data, int amount);

int RingBuffer_Empty(RingBuffer *r);
int RingBuffer_Full(RingBuffer *r);

int RingBuffer_Avaliable_Data(RingBuffer *r);
int RingBuffer_Avaliable_Space(RingBuffer *r);

bstring RingBuffer_Gets(RingBuffer *r, int amount);

#define RingBuffer_Avaliable_Data(B)                                           \
  (((B)->end + 1) % (B)->length - (B)->start - 1)

#define RingBuffer_Avaliable_Space(B) (B->length - (B)->end - 1)

#define RingBuffer_Full(B) (RingBuffer_Avaliable_Data((B)) - (B)->length == 0)

#define RingBuffer_Empty(B) (RingBuffer_Avaliable_Data((B)) == 0)

#define RingBuffer_Puts(B, D) RingBuffer_Wite((B), bdata((D)), bstrlen((D)))

#define RingBuffer_Get_All(B)                                                  \
  RingBuffer_Gets((B), RingBuffer_Avaliable_Data((B)))

#define RingBuffer_Starts_At(B) ((B)->buffer + (B)->start)

#define RingBuffer_Ends_At(B) ((B)->buffer + (B)->end)

#define RingBuffer_Commit_Read(B, A)                                           \
  ((B)->start = ((B)->start + (A)) % (B)->length)

#define RingBuffer_Commit_Write(B, A)                                          \
  ((B)->end = ((B)->end + (A)) % (B)->length)

#endif
