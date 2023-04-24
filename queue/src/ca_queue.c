#include <ca_queue.h>

ca_queue *ca_queue_new(size_t element_size, size_t initial_max) {
  return ca_vector_new(element_size, initial_max);
}

void ca_queue_destroy(ca_queue *s) {
  if (s->end > 0) {
    free(s->contents);
  }
  free(s);
}

void *ca_queue_recieve(ca_queue *s) {
  check(s->end > 0, "cannot recieve from empty");
  void *el = ca_vector_remove(s, 0);
  check(el != NULL, "element is NULL");
  s->contents++;
  s->end--;

  return el;
error:
  return NULL;
}
int ca_queue_send(ca_queue *s, void *value) { return ca_vector_push(s, value); }
void *ca_queue_peek(ca_queue *s) { return ca_vector_get(s, 0); }

int ca_queue_traverse(ca_queue *s, ca_queue_traverse_func func) {
  return ca_vector_traverse(s, func);
}
