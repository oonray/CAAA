#ifndef __vector_h
#define __vector_h
#include "dbg.h"
#include <assert.h>
#include <stdlib.h>

typedef int(ca_vector_Traverse_Func)(void *data);

typedef struct ca_vector {
  int end;
  int max;
  size_t element_size;
  size_t expand_rate;
  void **contents;
} ca_vector;

#define ca_vector_Last(A) ((A)->contens[(A)->end - 1])
#define ca_vector_First(A) ((A)->contents[0])
#define ca_vector_End(A) ((A)->end)
#define ca_vector_Max(A) ((A)->max)
#define ca_vector_Free(E) free((E))
#define DEFAULT_EXPAND 300

ca_vector *ca_vector_New(size_t element_size, size_t initial_max);
void ca_vector_Destroy(ca_vector *arr);

void ca_vector_Clear(ca_vector *arr);
int ca_vector_Resize(ca_vector *arr, size_t new_size);
int ca_vector_Expand(ca_vector *arr);
int ca_vector_Contract(ca_vector *arr);
int ca_vector_Push(ca_vector *arr, void *el);
void *ca_vector_Remove(ca_vector *arr, int i);
void *ca_vector_Pop(ca_vector *arr);
void ca_vector_Clear_Destroy(ca_vector *arr);
void ca_vector_Set(ca_vector *arr, int i, void *el);
void *ca_vector_Get(ca_vector *arr, int i);
void *ca_vector_Create(ca_vector *arr);
ca_vector *ca_vector_New_Data(size_t element_size, size_t initial_max,
                              void **data, int data_len);
int ca_vector_Traverse(ca_vector *v, ca_vector_Traverse_Func func);
int ca_vector_Traverse_N(ca_vector *v, int N, ca_vector_Traverse_Func func);
int ca_vector_Traverse_F(ca_vector *v, int F, ca_vector_Traverse_Func func);
int ca_vector_Traverse_F_T(ca_vector *v, int F, int T,
                           ca_vector_Traverse_Func func);
#endif
