#include "list.h"

List *List_New() {
  List *arr = (List *)malloc(sizeof(List));
  check_memory(arr);

  arr->length = 0;
  arr->first = NULL;
  arr->last = NULL;

  return arr;
error:
  if (arr) {
    free(arr);
  }
  return NULL;
}

List_Element *List_Element_New(size_t size,void *data){
  List_Element *out = calloc(1, sizeof(List_Element));
  check_memory(out);

  out->size = size;
  out->data = data;

  return out;
error:
    if(out){
      free(out);
      }
    return NULL;
}

void List_Destroy(List *arr) {
  List_Element *cur = arr->first;

  while(cur != NULL){
    List_Element *del = cur;
    cur = cur->next;
    List_Element_Destroy(del);
  }

  if(arr!=NULL)
    free(arr);
}

void List_Element_Destroy(List_Element *elm){
  if(elm == NULL){
    if(elm->data!=NULL)
      free(elm->data);

    free(elm);
  }
}

int List_Clear(List *arr) {
  List_Element *cur = arr->first;
  while(arr->first != NULL){
    List_Element *cur = List_Delete(arr,0);
    List_Element_Destroy(cur);
  }

  return 0;
error:
  return 1;
}

int List_Push(List *arr, size_t size,void *el) {
  List_Element *ell = List_Element_New(size,el);
  check(ell!=NULL,"Error creating vector element");

  if(arr->length <= 0){
    arr->length=0;
    arr->first=ell;
    arr->last=ell;
  }

  List_Element *lst = arr->last;

  lst->next = ell;
  arr->last = ell;
  ell->prev=lst;
  ell->next=NULL;

  arr->length++;

  return 0;
error:
  return 1;
}

List_Element *List_Pop(List *arr) {
  List_Element *el = List_Delete(arr,arr->length);
  return el;
error:
  return NULL;
}

List_Element *List_Delete(List *arr, int i){
  check(arr->length > 0,"Cannot delete on an empty vector");
  check(i<= arr->length,"Index out of range");
  List_Element *ell = NULL;

  if(arr->length<=1){
    ell=arr->first;
    arr->first = NULL;
    arr->last = NULL;
    arr->length--;
    return ell;
  }

  if(i==arr->length){
    ell = arr->last;
    arr->last = ell->prev;
    ell->prev->next=NULL;
    arr->length--;
    return ell;
  }

  ell = Find_List_Element(arr->first,i);
  check(ell!=NULL,"Element is NULL");

  if(ell == arr->first){
    arr->first=ell->next;
  }

  if(ell == arr->last){
    arr->last=ell->prev;
  }

  List_Element *nxt = ell->next;
  List_Element *prv = ell->prev;

  nxt->prev = prv;
  prv->next = nxt;

  arr->length--;

  return ell;
error:
  return NULL;
}

List_Element *Find_List_Element(List_Element *ell,int i){
  if(i==0){return ell;}
  return Find_List_Element(ell->next,i-1);
}

int List_Set(List *arr, int i, size_t size, void *el) {
  check(i<=arr->length,"Not enought elements has %d got %d",arr->length,i);
  List_Element *ell = Find_List_Element(arr->first,i);
  check(ell!=NULL,"Could not get ellement");
  ell->size = size;
  ell->data = el;
  return 0;
error:
  return 1;
}

List_Element *List_Get(List *arr, int i) {
  check(i<=arr->length,"Not enought elements has %d got %d",arr->length,i);
  List_Element *ell = Find_List_Element(arr->first,i);
  check(ell!=NULL,"Could not get ellement");
  return ell;
error:
  return NULL;
}
