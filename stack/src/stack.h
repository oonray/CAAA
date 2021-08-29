#include <c/vector.h>

#ifndef __stack_h
#define __stack_h

typedef Vector Stack;	
typedef Vector_Traverse_Func Stack_Traverse_Func;

#define Stack_Count(A) (A)->end
Stack *Stack_New(size_t element_size,size_t initial_max);
void Stack_Destroy(Stack *s);
void *Stack_Pop(Stack *s);
int Stack_Push(Stack *s,void *value);
void *Stack_Peek(Stack *s);
int Stack_Traverse(Stack *s,Stack_Traverse_Func func);
#endif
