#ifndef __ca_bintree_h
#define __ca_bintree_h

#include <bstring/bstrlib.h>
#include <ca_dbg.h>
#include <stdlib.h>

typedef int (*ca_bintree_compare)(void *a, void *b);

typedef struct ca_bintreenode {
  void *key;
  void *data;

  struct ca_bintreenode *left;
  struct ca_bintreenode *right;
  struct ca_bintreenode *parent;
} ca_bintreenode;

typedef struct ca_bintree {
  int count;
  ca_bintree_compare compare;
  ca_bintreenode *root;
} ca_bintree;

typedef int (*ca_bintree_traverse_cb)(ca_bintreenode *node);

ca_bintree *ca_bintree_create(ca_bintree_compare compare);
void ca_bintree_destroy(ca_bintree *bin);

int ca_bintree_set(ca_bintree *bin, void *key, void *data);
void *ca_bintree_get(ca_bintree *bin, void *key);

int ca_bintree_traverse(ca_bintree *bin, ca_bintree_traverse_cb trav);
void *ca_bintree_delete(ca_bintree *map, void *key);

#endif
