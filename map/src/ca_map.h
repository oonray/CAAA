#ifndef __ca_map_h
#define __ca_map_h

#include <bstring/bstrlib.h>
#include <ca_dbg.h>
#include <ca_vector.h>
#include <stdint.h>

#define DEFAULT_BUCKET_NUM 100

typedef int (*ca_map_compare)(void *a, void *b);
typedef uint32_t (*ca_map_hash)(void *key);

typedef struct ca_map {
  ca_vector *buckets;
  ca_map_compare compare;
  ca_map_hash hash;
} ca_map;

typedef struct ca_map_node {
  void *key;
  void *data;
  uint32_t hash;
} ca_map_node;

typedef int (*ca_map_travese_func)(ca_map_node *node);

ca_map *ca_map_new(ca_map_compare compare, ca_map_hash hash);
void ca_map_destroy(ca_map *m);

int ca_map_set(ca_map *m, void *key, void *data);
void *ca_map_get(ca_map *m, void *key);
void *ca_map_delete(ca_map *m, void *key);

int ca_map_travese(ca_map *m, ca_map_travese_func func);

static int default_compare(void *a, void *b);
static uint32_t default_hash(void *key);
#endif
