#include "vector.h"

ca_vector *ca_vector_New(size_t element_size, size_t initial_max) {
  ca_vector *arr = (ca_vector *)malloc(sizeof(ca_vector));
  check_memory(arr);

  arr->max = initial_max;
  check(arr->max > 0, "There must be an initial max");

  arr->contents = calloc(initial_max, sizeof(void *));
  check_memory(arr->contents);

  arr->end = 0;
  arr->element_size = element_size;

  arr->expand_rate = DEFAULT_EXPAND;
  return arr;
error:
  if (arr) {
    free(arr);
  }
  return NULL;
}

void ca_vector_Destroy(ca_vector *arr) {
  if (arr) {
    if (arr->contents) {
      free(arr->contents);
      free(arr);
    }
  }
}

void ca_vector_Clear(ca_vector *arr) {
  int i = 0;
  if (arr->element_size > 0) {
    for (i = 0; i < arr->max; i++) {
      if (arr->contents[i] != NULL) {
        free(arr->contents[i]);
      }
    }
  }
}

int ca_vector_Resize(ca_vector *arr, size_t new_size) {
  arr->max = new_size;
  check(arr->max > 0, "The max size must be grater than 0");

  void *content = realloc(arr->contents, arr->max * sizeof(void *));
  check_memory(content);

  arr->contents = content;
  return 0;
error:
  return -1;
}

int ca_vector_Expand(ca_vector *arr) {
  size_t old_max = arr->max;
  int exp = arr->max + arr->expand_rate;

  check(ca_vector_Resize(arr, exp) == 0, "Faild to expand vector to %d", exp);
  memset(arr->contents + old_max, 0, arr->expand_rate + 1);
  return 0;
error:
  return -1;
}

int ca_vector_Contract(ca_vector *arr) {
  int new_size =
      arr->end < (int)arr->expand_rate ? (int)arr->expand_rate : arr->end;
  return ca_vector_Resize(arr, new_size + 1);
}

int ca_vector_Push(ca_vector *arr, void *el) {
  arr->contents[arr->end] = el;
  arr->end++;
  if (ca_vector_End(arr) >= ca_vector_Max(arr)) {
    return ca_vector_Expand(arr);
  }
  return 0;
}

void *ca_vector_Remove(ca_vector *arr, int i) {
  void *el = arr->contents[i];
  arr->contents[i] = NULL;
  return el;
}

void *ca_vector_Pop(ca_vector *arr) {
  check(arr->end - 1 >= 0, "Cannot pop from Empty");
  void *el = ca_vector_Remove(arr, arr->end - 1);
  arr->end--;

  if ((ca_vector_End(arr) > (int)arr->expand_rate) &&
      (ca_vector_End(arr) % arr->expand_rate)) {
    ca_vector_Contract(arr);
  }
  return el;
error:
  return NULL;
}

void ca_vector_Clear_Destroy(ca_vector *arr) {
  ca_vector_Clear(arr);
  ca_vector_Destroy(arr);
}

void ca_vector_Set(ca_vector *arr, int i, void *el) {
  check(i < arr->max, "Attempt to set beyond max");
  if (i > arr->end) {
    arr->end = i;
  }
  arr->contents[i] = el;
error:
  return;
}

void *ca_vector_Get(ca_vector *arr, int i) {
  check(i < arr->max, "Atempting to get beyond max");
  return arr->contents[i];
error:
  return NULL;
}

void *ca_vector_Create(ca_vector *arr) {
  check(arr->element_size > 0, "Cant Create 0 size ca_vector");
  return calloc(1, arr->element_size);
error:
  return NULL;
}

ca_vector *ca_vector_New_Data(size_t element_size, size_t initial_max,
                              void **data, int data_len) {
  ca_vector *v = ca_vector_New(element_size, initial_max);
  while (v->max < data_len) {
    ca_vector_Expand(v);
  }
  v->contents = data;
  v->end += data_len;
  return v;
}

int ca_vector_Traverse(ca_vector *v, ca_vector_Traverse_Func func) {
  int counter = 0;
  check_memory(v);
  for (int i = 0; i < v->end; i++) {
    func(v->contents[i]);
    counter++;
  }

  return counter;
error:
  return counter;
}

int ca_vector_Traverse_N(ca_vector *v, int N, ca_vector_Traverse_Func func) {
  int counter = 0;
  check_memory(v);
  for (int i = 0; i < N; i++) {
    func(v->contents[i]);
    counter++;
  }

  return counter;
error:
  return counter;
}

int ca_vector_Traverse_F(ca_vector *v, int F, ca_vector_Traverse_Func func) {
  int counter = 0;
  check_memory(v);
  for (int i = F; i < v->end; i++) {
    func(v->contents[i]);
    counter++;
  }

  return counter;
error:
  return counter;
}

int ca_vector_Traverse_F_T(ca_vector *v, int F, int T,
                           ca_vector_Traverse_Func func) {
  int counter = 0;
  check_memory(v);
  for (int i = F; i < T; i++) {
    func(v->contents[i]);
    counter++;
  }

  return counter;
error:
  return counter;
}
