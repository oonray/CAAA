#ifndef _ca_tritree_h
#define _ca_tritree_h

#include <bstring/bstrlib.h>
#include <ca_vector.h>
#include <stdlib.h>

typedef struct ca_tritree {
  char splitchar;
  struct ca_tritree *low;
  struct ca_tritree *equal;
  struct ca_tritree *high;
  void *value;
} ca_tritree;

typedef void (*ca_tritree_traverse_func)(void *value, void *data);

ca_tritree *ca_tritree_insert(ca_tritree *node, const char *key, size_t len,
                              void *value);

void *ca_tritree_search(ca_tritree *root, const char *key, size_t len);
void *ca_tritree_search_prefix(ca_tritree *root, const char *key, size_t len);

void ca_tritree_traverse(ca_tritree *node, ca_tritree_traverse_func func,
                         void *data);
void ca_tritree_destroy(ca_tritree *root);
#endif
