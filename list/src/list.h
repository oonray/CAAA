#ifndef __vector_h
#define __vector_h
#include "dbg.h"
#include "assert.h"
#include <stdlib.h>

typedef int(List_Traverse_Func)(void *data);

typedef struct List_Element{
  size_t size;
  struct List_Element *next;
  struct List_Element *prev;
  void *data;
}List_Element;

typedef struct List {
  int length;
  List_Element *first;
  List_Element *last;
} List;

#define List_Last(A) ((A)->last)
#define List_Last_Data(A) ((A)->last->data)
#define List_First(A) ((A)->first)
#define List_First_Data(A) ((A)->first->data)
#define List_End(A) A->length
#define List_Value(A) ((List_Element *)A)->data
#define DEFAULT_EXPAND 300

List *List_New();
List_Element *List_Element_New(size_t size,void *data);

void List_Destroy(List *arr);
List_Element *Find_List_Element(List_Element *ell,int i);

int List_Clear(List *arr);
int List_Push(List *arr, size_t size,void *el);
List_Element *List_Pop(List *arr);
List_Element *List_Delete(List *arr, int i);

int List_Set(List *arr, int i, size_t size, void *el);
List_Element *List_Get(List *arr, int i);
#endif
