#include <ca_bintree.h>

static int default_compare(void *a, void *b) {
  return bstrcmp((bstring)a, (bstring)b);
}

ca_bintree *ca_bintree_create(ca_bintree_compare compare) {
  ca_bintree *map = calloc(1, sizeof(ca_bintree));
  check_memory(map);

  map->compare = compare == NULL ? default_compare : compare;
  return map;
error:
  if (map)
    ca_bintree_destroy(map);
  return NULL;
}

static int ca_bintree_destroy_cb(ca_bintreenode *node) {
  free(node);
  return 0;
}

void ca_bintree_destroy(ca_bintree *bin) {
  if (bin) {
    ca_bintree_traverse(bin, ca_bintree_destroy_cb);
    free(bin);
  }
}

static inline ca_bintreenode *ca_bintreenode_create(ca_bintreenode *parent,
                                                    void *key, void *data) {
  ca_bintreenode *node = calloc(1, sizeof(ca_bintreenode));
  check_memory(node);

  node->key = key;
  node->data = data;
  node->parent = parent;
  return node;

error:
  return NULL;
}
static inline void ca_bintree_setnode(ca_bintree *map, ca_bintreenode *node,
                                      void *key, void *data) {
  int cmp = map->compare(node->key, key);
  if (cmp <= 0) {
    if (node->left) {
      ca_bintree_setnode(map, node->left, key, data);
    } else {
      node->left = ca_bintreenode_create(node, key, data);
    }
  } else {
    if (node->right) {
      ca_bintree_setnode(map, node->right, key, data);
    } else {
      node->right = ca_bintreenode_create(node, key, data);
    }
  }
}

int ca_bintree_set(ca_bintree *map, void *key, void *data) {
  if (map->root == NULL) {
    map->root = ca_bintreenode_create(NULL, key, data);
    check_memory(map->root);
  } else {
    ca_bintree_setnode(map, map->root, key, data);
  }
  return 0;
error:
  return -1;
}

static inline ca_bintreenode *
ca_bintreenode_getnode(ca_bintree *map, ca_bintreenode *node, void *key) {
  int cmp = map->compare(node->key, key);
  if (cmp == 0) {
    return node;
  } else if (cmp < 0) {
    if (node->left) {
      return ca_bintreenode_getnode(map, node->left, key);
    } else {
      return NULL;
    }
  } else {
    if (node->right) {
      return ca_bintreenode_getnode(map, node->right, key);
    } else {
      return NULL;
    }
  }
}

void *ca_bintree_get(ca_bintree *map, void *key) {
  if (map->root == NULL) {
    return NULL;
  } else {
    ca_bintreenode *node = ca_bintreenode_getnode(map, map->root, key);
    return node == NULL ? NULL : node->data;
  }
}

static inline int
ca_bintree_traverse_nodes(ca_bintreenode *node,
                          ca_bintree_traverse_cb traverse_cb) {
  int rc = 0;
  if (node->left) {
    rc = ca_bintree_traverse_nodes(node->left, traverse_cb);
    if (rc != 0) {
      return rc;
    }
  }
  if (node->right) {
    rc = ca_bintree_traverse_nodes(node->right, traverse_cb);
    if (rc != 0) {
      return rc;
    }
  }

  return traverse_cb(node);
}

int ca_bintree_traverse(ca_bintree *map, ca_bintree_traverse_cb traverse_cb) {
  if (map->root) {
    return ca_bintree_traverse_nodes(map->root, traverse_cb);
  }
  return 0;
}

static inline ca_bintreenode *ca_bintree_find_min(ca_bintreenode *node) {
  while (node->left) {
    node = node->left;
  }
  return node;
}

static inline void
ca_bintree_replace_node_in_parent(ca_bintree *map, ca_bintreenode *node,
                                  ca_bintreenode *new_value) {
  if (node->parent) {
    if (node == node->parent->left) {
      node->parent->left = new_value;
    } else {
      node->parent->right = new_value;
    }
  } else {
    map->root = new_value;
  }

  if (new_value) {
    new_value->parent = node->parent;
  }
}

static inline void ca_bintree_swap(ca_bintreenode *a, ca_bintreenode *b) {
  void *tmp = NULL;
  tmp = b->key;

  b->key = a->key;
  a->key = tmp;

  tmp = b->data;
  b->data = a->data;
  a->data = tmp;
}

static inline ca_bintreenode *
ca_bintreenode_delete(ca_bintree *map, ca_bintreenode *node, void *key) {
  int cmp = map->compare(node->key, key);
  if (cmp < 0) {
    if (node->left) {
      return ca_bintreenode_delete(map, node->left, key);
    } else {
      return NULL;
    }
  } else if (cmp > 0) {
    if (node->right) {
      return ca_bintreenode_delete(map, node->right, key);
    } else {
      return NULL;
    }
  } else {
    if (node->left && node->right) {
      ca_bintreenode *successor = ca_bintree_find_min(node->right);
      ca_bintree_swap(successor, node);
      ca_bintree_replace_node_in_parent(map, successor, successor->right);
      return successor;
    } else if (node->left) {
      ca_bintree_replace_node_in_parent(map, node, node->left);
    } else if (node->right) {
      ca_bintree_replace_node_in_parent(map, node, node->right);
    } else {
      ca_bintree_replace_node_in_parent(map, node, NULL);
    }

    return node;
  }
}

void *ca_bintree_delete(ca_bintree *map, void *key) {
  void *data = NULL;
  if (map->root) {
    ca_bintreenode *node = ca_bintreenode_delete(map, map->root, key);
    if (node) {
      data = node->data;
      free(node);
    }
  }

  return data;
}
