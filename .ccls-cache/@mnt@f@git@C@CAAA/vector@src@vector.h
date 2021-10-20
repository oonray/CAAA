#ifndef __vector_h
#define __vector_h
#include <stdlib.h>
#include <assert.h>
#include <dbg.h>


typedef struct VectorNode VectorNode;

struct VectorNode {
	void *data;
	VectorNode *next;
};

typedef VectorNode *(Vector_Traverse_Func)(VectorNode *data);

typedef struct Vector {
	int size;
	VectorNode *first;
} Vector;

VectorNode *Vector_Node_New();
Vector *Vector_New();
void Vector_Destroy(Vector *arr);

int Vector_Add(Vector *v,void *data);
int Vector_Set(Vector *v,int n,void *data);
void *Vector_Del(Vector *v,int n);
void *Vector_Get(Vector *v,int n);
void *Vector_Get_Last(Vector *v);
void *Vector_Apply(Vector *v);
Vector *Vector_Traverse(Vector *v,Vector_Traverse_Func *f);
#endif
