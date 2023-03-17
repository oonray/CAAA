#include <ca_dbg.h>
#include <ca_tritree.h>

#include <stdio.h>
#include <stdlib.h>

static inline ca_tritree *ca_tritree_insert_base(ca_tritree *root,
                                                 ca_tritree *node,
                                                 const char *key, size_t len,
                                                 void *value) {
  if (node == NULL) {
    node = (ca_tritree *)calloc(1, sizeof(ca_tritree));

    if (root == NULL) {
      root = node;
    }

    node->splitchar = *key;
  }

  if (*key < node->splitchar) {
    node->low = ca_tritree_insert_base(root, node->low, key, len, value);
  } else if (*key == node->splitchar) {
    if (len > 1) {
      node->equal =
          ca_tritree_insert_base(root, node->equal, key + 1, len - 1, value);
    } else {
      assert(node->value == NULL);
      node->value = value;
    }
  } else {
    node->high = ca_tritree_insert_base(root, node->high, key, len, value);
  }

  return node;
}

ca_tritree *ca_tritree_insert(ca_tritree *node, const char *key, size_t len,
                              void *value) {
  return ca_tritree_insert_base(node, node, key, len, value);
}

void *ca_tritree_search(ca_tritree *root, const char *key, size_t len) {
  ca_tritree *node = root;
  size_t i = 0;
  while (i < len && node) {
    if (key[i] < node->splitchar) {
      node = node->low;
    } else if (key[i] == node->splitchar) {
      i++;
      if (i < len)
        node = node->equal;
    } else {
      node = node->high;
    }
  }

  if (node) {
    return node->value;
  } else {
    return NULL;
  }
}

void *ca_tritree_search_prefix(ca_tritree *root, const char *key, size_t len) {
  if (len <= 0)
    return NULL;

  ca_tritree *node = root;
  ca_tritree *last = NULL;
  size_t i = 0;

  while (i < len && node) {
    if (key[i] < node->splitchar) {
      node = node->low;
    } else if (key[i] == node->splitchar) {
      i++;
      if (i < len) {
        if (node->value)
          last = node;
        node = node->equal;
      }
    } else {
      node = node->high;
    }
  }

  node = node ? node : last;

  while (node && !node->value) {
    node = node->equal;
  }

  return node ? node->value : NULL;
}

void ca_tritree_traverse(ca_tritree *node, ca_tritree_traverse_func func,
                         void *data) {
  if (!node)
    return;

  if (node->low)
    ca_tritree_traverse(node->low, func, data);

  if (node->equal)
    ca_tritree_traverse(node->equal, func, data);

  if (node->high)
    ca_tritree_traverse(node->high, func, data);

  if (node->value)
    func(node->value, data);
}

void ca_tritree_destroy(ca_tritree *node) {
  if (node == NULL)
    return;

  if (node->low)
    ca_tritree_destroy(node->low);

  if (node->equal)
    ca_tritree_destroy(node->low);

  if (node->high)
    ca_tritree_destroy(node->high);

  free(node);
}
