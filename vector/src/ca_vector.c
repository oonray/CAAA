#include <ca_vector.h>

ca_vector *ca_vector_new(size_t element_size, size_t initial_max) {
  ca_vector *arr = (ca_vector *)malloc(sizeof(ca_vector));
  check_memory(arr);

  arr->max = initial_max;
  check(arr->max > 0, "there must be an initial max");

  arr->contents = calloc(initial_max, sizeof(void *));
  check_memory(arr->contents);

  arr->end = 0;
  arr->element_size = element_size;

  arr->expand_rate = default_expand;
  return arr;
error:
  if (arr) {
    free(arr);
  }
  return NULL;
}

void ca_vector_destroy(ca_vector *arr) {
  if (arr) {
    if (arr->contents) {
      free(arr->contents);
      free(arr);
    }
  }
}

void ca_vector_clear(ca_vector *arr) {
  int i = 0;
  if (arr->element_size > 0) {
    for (i = 0; i < arr->max; i++) {
      if (arr->contents[i] != NULL) {
        free(arr->contents[i]);
      }
    }
  }
}

int ca_vector_resize(ca_vector *arr, size_t new_size) {
  arr->max = new_size;
  check(arr->max > 0, "the max size must be grater than 0");

  void *content = realloc(arr->contents, arr->max * sizeof(void *));
  check_memory(content);

  arr->contents = content;
  return 0;
error:
  return -1;
}

int ca_vector_expand(ca_vector *arr) {
  size_t old_max = arr->max;
  int exp = arr->max + arr->expand_rate;

  check(ca_vector_resize(arr, exp) == 0, "faild to expand vector to %d", exp);
  memset(arr->contents + old_max, 0, arr->expand_rate + 1);
  return 0;
error:
  return -1;
}

int ca_vector_contract(ca_vector *arr) {
  int new_size =
      arr->end < (int)arr->expand_rate ? (int)arr->expand_rate : arr->end;
  return ca_vector_resize(arr, new_size + 1);
}

int ca_vector_push(ca_vector *arr, void *el) {
  arr->contents[arr->end] = el;
  arr->end++;
  if (ca_vector_end(arr) >= ca_vector_max(arr)) {
    return ca_vector_expand(arr);
  }
  return 0;
}

void *ca_vector_remove(ca_vector *arr, int i) {
  void *el = arr->contents[i];
  arr->contents[i] = NULL;
  return el;
}

void *ca_vector_pop(ca_vector *arr) {
  check(arr->end - 1 >= 0, "cannot pop from empty");
  void *el = ca_vector_remove(arr, arr->end - 1);
  arr->end--;

  if ((ca_vector_end(arr) > (int)arr->expand_rate) &&
      (ca_vector_end(arr) % arr->expand_rate)) {
    ca_vector_contract(arr);
  }
  return el;
error:
  return NULL;
}

void ca_vector_clear_destroy(ca_vector *arr) {
  ca_vector_clear(arr);
  ca_vector_destroy(arr);
}

void ca_vector_set(ca_vector *arr, int i, void *el) {
  check(i < arr->max, "attempt to set beyond max");
  if (i > arr->end) {
    arr->end = i;
  }
  arr->contents[i] = el;
error:
  return;
}

void *ca_vector_get(ca_vector *arr, int i) {
  check(i < arr->max, "atempting to get beyond max");
  return arr->contents[i];
error:
  return NULL;
}

void *ca_vector_create(ca_vector *arr) {
  check(arr->element_size > 0, "cant create 0 size ca_vector");
  return calloc(1, arr->element_size);
error:
  return NULL;
}

ca_vector *ca_vector_new_data(size_t element_size, size_t initial_max,
                              void **data, int data_len) {
  ca_vector *v = ca_vector_new(element_size, initial_max);
  while (v->max < data_len) {
    ca_vector_expand(v);
  }
  v->contents = data;
  v->end += data_len;
  return v;
}

int ca_vector_traverse(ca_vector *v, ca_vector_traverse_func func) {
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

int ca_vector_traverse_n(ca_vector *v, int n, ca_vector_traverse_func func) {
  int counter = 0;
  check_memory(v);
  for (int i = 0; i < n; i++) {
    func(v->contents[i]);
    counter++;
  }

  return counter;
error:
  return counter;
}

int ca_vector_traverse_f(ca_vector *v, int f, ca_vector_traverse_func func) {
  int counter = 0;
  check_memory(v);
  for (int i = f; i < v->end; i++) {
    func(v->contents[i]);
    counter++;
  }

  return counter;
error:
  return counter;
}

int ca_vector_traverse_f_t(ca_vector *v, int f, int t,
                           ca_vector_traverse_func func) {
  int counter = 0;
  check_memory(v);
  for (int i = f; i < t; i++) {
    func(v->contents[i]);
    counter++;
  }

  return counter;
error:
  return counter;
}
