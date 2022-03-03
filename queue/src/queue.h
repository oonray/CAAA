#include <caaa/vector.h>

#ifndef __queue_h
#define __queue_h

typedef Vector Queue;
typedef Vector_Traverse_Func Queue_Traverse_Func;

#define Queue_Count(A) (A)->end
Queue *Queue_New(size_t element_size, size_t initial_max);
void Queue_Destroy(Queue *s);
void *Queue_Recieve(Queue *s);
int Queue_Send(Queue *s, void *value);
void *Queue_Peek(Queue *s);
int Queue_Traverse(Queue *s, Queue_Traverse_Func func);

#endif
