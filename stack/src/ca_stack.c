#include <ca_stack.h>

ca_stack *ca_stack_new(size_t element_size, size_t initial_max) {
  return ca_vector_new(element_size, initial_max);
}

void ca_stack_destroy(ca_stack *s) { ca_vector_destroy(s); }

void *ca_stack_pop(ca_stack *s) { return ca_vector_pop(s); }
int ca_stack_push(ca_stack *s, void *value) { return ca_vector_push(s, value); }
void *ca_stack_peek(ca_stack *s) { return ca_vector_get(s, s->end - 1); }

int ca_stack_traverse(ca_stack *s, ca_stack_traverse_func func) {
  return ca_vector_traverse(s, func);
}
