#ifndef __vector_h
#define __vector_h
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <c/dbg.h>

typedef int (Vector_Traverse_Func)(void *data);

typedef struct Vector {
	int end;
	int max;
	size_t element_size;
	size_t expand_rate;
	void **contents;
} Vector;

#define Vector_Last(A) ((A)->contens[(A)->end-1])
#define Vector_First(A) ((A)->contents[0])
#define Vector_End(A) ((A)->end)
#define Vector_Max(A) ((A)->max)
#define Vector_Free(E) free((E))
#define DEFAULT_EXPAND 300


Vector *Vector_New(size_t element_size,size_t initial_max);
void Vector_Destroy(Vector *arr);

void Vector_Clear(Vector *arr);
int Vector_Resize(Vector *arr,size_t new_size);
int Vector_Expand(Vector *arr);
int Vector_Contract(Vector *arr);
int Vector_Push(Vector *arr,void *el);
void *Vector_Remove(Vector *arr,int i);
void *Vector_Pop(Vector *arr);
void Vector_Clear_Destroy(Vector *arr);
void Vector_Set(Vector *arr,int i, void *el);
void *Vector_Get(Vector *arr,int i);
void *Vector_Create(Vector *arr);
Vector *Vector_New_Data(size_t element_size,size_t initial_max,void **data,int data_len);
int Vector_Traverse(Vector *v,Vector_Traverse_Func func);
int Vector_Traverse_N(Vector *v, int N,Vector_Traverse_Func func);
int Vector_Traverse_F(Vector *v, int F,Vector_Traverse_Func func);
int Vector_Traverse_F_T(Vector *v, int F,int T,Vector_Traverse_Func func);
#endif
