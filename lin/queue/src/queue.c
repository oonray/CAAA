#include "queue.h"

Queue *Queue_New(size_t element_size,size_t initial_max){
	return Vector_New(element_size,initial_max);
}

void Queue_Destroy(Queue *s){
	if(s->end > 0){
		free(s->contents);
	}
	free(s);
}

void *Queue_Recieve(Queue *s){
		check(s->end>0,"Cannot recieve from Empty");	
		void*el = Vector_Remove(s,0);
		check(el != NULL,"Element is NULL");
		s->contents++;
		s->end--;

		return el;
error:
		return NULL;
}
int Queue_Send(Queue *s,void *value){return Vector_Push(s,value);}
void *Queue_Peek(Queue *s){return Vector_Get(s,0);}

int Queue_Traverse(Queue *s,Queue_Traverse_Func func){
	return Vector_Traverse(s,func);	
}

