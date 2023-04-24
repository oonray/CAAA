#include <ca_vector.h>

#ifndef __ca_queue_h
#define __ca_queue_h

typedef ca_vector ca_queue;
typedef ca_vector_traverse_func ca_queue_traverse_func;

#define ca_queue_count(a) (a)->end
ca_queue *ca_queue_new(size_t element_size, size_t initial_max);
void ca_queue_destroy(ca_queue *s);
void *ca_queue_recieve(ca_queue *s);
int ca_queue_send(ca_queue *s, void *value);
void *ca_queue_peek(ca_queue *s);
int ca_queue_traverse(ca_queue *s, ca_queue_traverse_func func);

#endif
