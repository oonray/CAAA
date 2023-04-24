#include <ca_vector.h>

#ifndef __ca_stack_h
#define __ca_stack_h

typedef ca_vector ca_stack;
typedef ca_vector_traverse_func ca_stack_traverse_func;

#define ca_stack_count(a) (a)->end
ca_stack *ca_stack_new(size_t element_size, size_t initial_max);
void ca_stack_destroy(ca_stack *s);
void *ca_stack_pop(ca_stack *s);
int ca_stack_push(ca_stack *s, void *value);
void *ca_stack_peek(ca_stack *s);
int ca_stack_traverse(ca_stack *s, ca_stack_traverse_func func);
#endif
