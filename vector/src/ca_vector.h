#ifndef __ca_vector_h
#define __ca_vector_h
#include <assert.h>
#include <ca_dbg.h>
#include <stdlib.h>

typedef int(ca_vector_traverse_func)(void *data);

typedef struct ca_vector {
  int end;
  int max;
  size_t element_size;
  size_t expand_rate;
  void **contents;
} ca_vector;

#define ca_vector_last(a) ((a)->contens[(a)->end - 1])
#define ca_vector_first(a) ((a)->contents[0])
#define ca_vector_end(a) ((a)->end)
#define ca_vector_max(a) ((a)->max)
#define ca_vector_free(e) free((e))
#define default_expand 300

ca_vector *ca_vector_new(size_t element_size, size_t initial_max);
void ca_vector_destroy(ca_vector *arr);

void ca_vector_clear(ca_vector *arr);
int ca_vector_resize(ca_vector *arr, size_t new_size);
int ca_vector_expand(ca_vector *arr);
int ca_vector_contract(ca_vector *arr);
int ca_vector_push(ca_vector *arr, void *el);
void *ca_vector_remove(ca_vector *arr, int i);
void *ca_vector_pop(ca_vector *arr);
void ca_vector_clear_destroy(ca_vector *arr);
void ca_vector_set(ca_vector *arr, int i, void *el);
void *ca_vector_get(ca_vector *arr, int i);
void *ca_vector_create(ca_vector *arr);
ca_vector *ca_vector_new_data(size_t element_size, size_t initial_max,
                              void **data, int data_len);
int ca_vector_traverse(ca_vector *v, ca_vector_traverse_func func);
int ca_vector_traverse_n(ca_vector *v, int n, ca_vector_traverse_func func);
int ca_vector_traverse_f(ca_vector *v, int f, ca_vector_traverse_func func);
int ca_vector_traverse_f_t(ca_vector *v, int f, int t,
                           ca_vector_traverse_func func);
#endif
