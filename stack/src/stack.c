#include "stack.h"

Stack *Stack_New(size_t element_size,size_t initial_max){
	return Vector_New(element_size,initial_max);
}

void Stack_Destroy(Stack *s){
	Vector_Destroy(s);
}

void *Stack_Pop(Stack *s){return Vector_Pop(s);}
int Stack_Push(Stack *s,void *value){return Vector_Push(s,value);}
void *Stack_Peek(Stack *s){return Vector_Get(s,s->end-1);}

int Stack_Traverse(Stack *s,Stack_Traverse_Func func){
	return Vector_Traverse(s,func);	
}

