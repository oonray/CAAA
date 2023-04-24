#include <ca_map.h>

static int default_compare(void *a, void *b) {
  return bstrcmp((bstring)a, (bstring)b);
}

static uint32_t default_hash(void *key) {
  size_t len = blength((bstring)key);
  char *k = bdata((bstring)key);

  uint32_t hash = 0;
  uint32_t i = 0;

  for (hash = i = 0; i < len; ++i) {
    hash += k[i];
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }

  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);
  return hash;
}

ca_map *ca_map_new(ca_map_compare compare, ca_map_hash hash) {
  ca_map *map = (ca_map *)calloc(1, sizeof(ca_map));
  check_memory(map);

  map->compare = compare == NULL ? default_compare : compare;
  map->hash = compare == NULL ? default_hash : hash;

  map->buckets = ca_vector_new(sizeof(ca_vector *), DEFAULT_BUCKET_NUM);
  check_memory(map->buckets);

  map->buckets->end = map->buckets->max;
  return map;

error:
  if (map) {
    ca_map_destroy(map);
  }
  return NULL;
}

void ca_map_destroy(ca_map *m) {
  int i = 0;
  int j = 0;
  if (m) {
    if (m->buckets) {
      for (i = 0; i < ca_vector_end(m->buckets); i++) {
        ca_vector *bucket = (ca_vector *)ca_vector_get(m->buckets, i);
        if (bucket) {
          for (j = 0; j < ca_vector_end(bucket); j++) {
            free(ca_vector_get(bucket, j));
          }
          ca_vector_destroy(bucket);
        }
      }
      ca_vector_destroy(m->buckets);
    }
    free(m);
  }
}

static inline ca_map_node *ca_map_node_new(int hash, void *key, void *data) {
  ca_map_node *node = (ca_map_node *)calloc(1, sizeof(ca_map_node));
  check_memory(node);

  node->key = key;
  node->data = data;
  node->hash = hash;

  return node;
error:
  return NULL;
}

static inline ca_vector *ca_map_find_bucket(ca_map *m, void *key, int create,
                                            uint32_t *hash_out) {
  uint32_t h = m->hash(key);
  int bucket_n = h % DEFAULT_BUCKET_NUM;
  check(bucket_n >= 0, "invalid bucket got %d", bucket_n);

  *hash_out = h;
  ca_vector *bucket = ca_vector_get(m->buckets, bucket_n);

  if ((bucket == NULL) && create) {
    bucket = ca_vector_new(sizeof(void *), DEFAULT_BUCKET_NUM);
    check(bucket != NULL, "could not create bucket");
    ca_vector_set(m->buckets, bucket_n, bucket);
  }

  return bucket;
error:
  return NULL;
}

int ca_map_set(ca_map *m, void *key, void *data) {
  uint32_t h = 0;

  ca_vector *bucket = ca_map_find_bucket(m, key, 1, &h);
  check(bucket != NULL, "did not find or create bucket");

  ca_map_node *node = ca_map_node_new(h, key, data);
  check(node != NULL, "could not create node");

  ca_vector_push(bucket, node);

  return 0;
error:
  return -1;
}

static inline int ca_map_get_node(ca_map *m, uint32_t hash, ca_vector *bucket,
                                  void *key) {
  int i = 0;
  for (i = 0; i < ca_vector_end(bucket); i++) {
    ca_map_node *node = (ca_map_node *)ca_vector_get(bucket, i);
    if (node->hash == hash && m->compare(node->key, key) == 0) {
      return i;
    }
  }
  return -1;
}

void *ca_map_get(ca_map *m, void *key) {
  uint32_t hash = 0;
  ca_vector *bucket = ca_map_find_bucket(m, key, 0, &hash);
  if (!bucket)
    return NULL;

  int i = ca_map_get_node(m, hash, bucket, key);
  if (i == -1)
    return NULL;

  ca_map_node *node = (ca_map_node *)ca_vector_get(bucket, i);
  check(node != NULL, "failed to get node from bucket.");

  return node->data;
error:
  return NULL;
}

int ca_map_travese(ca_map *m, ca_map_travese_func func) {
  int i = 0;
  int j = 0;
  int rc = 0;

  for (i = 0; i < ca_vector_end(m->buckets); i++) {
    ca_vector *bucket = (ca_vector *)ca_vector_get(m->buckets, i);
    if (bucket) {
      for (j = 0; j < ca_vector_end(bucket); j++) {
        ca_map_node *node = (ca_map_node *)ca_vector_get(bucket, j);
        rc = func(node);
        if (rc != 0) {
          return rc;
        }
      }
    }
  }
  return 0;
}

void *ca_map_delete(ca_map *m, void *key) {
  uint32_t hash = 0;
  ca_vector *bucket = ca_map_find_bucket(m, key, 0, &hash);
  if (!bucket)
    return NULL;

  int i = ca_map_get_node(m, hash, bucket, key);
  if (i == -1)
    return NULL;

  ca_map_node *node = (ca_map_node *)ca_vector_get(bucket, i);
  void *data = node->data;
  free(node);

  ca_map_node *ending = (ca_map_node *)ca_vector_pop(bucket);
  if (ending != node) {
    ca_vector_set(bucket, i, ending);
  }

  return data;

error:
  return NULL;
}
