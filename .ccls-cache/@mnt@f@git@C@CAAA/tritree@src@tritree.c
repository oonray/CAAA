#include "tritree.h"
#include "dbg.h"

#include <stdio.h>
#include <stdlib.h>

static inline TriTree *TriTree_Insert_Base(TriTree *root, TriTree *node,
                                           const char *key, size_t len,
                                           void *value) {
  if (node == NULL) {
    node = (TriTree *)calloc(1, sizeof(TriTree));

    if (root == NULL) {
      root = node;
    }

    node->splitchar = *key;
  }

  if (*key < node->splitchar) {
    node->low = TriTree_Insert_Base(root, node->low, key, len, value);
  } else if (*key == node->splitchar) {
    if (len > 1) {
      node->equal =
          TriTree_Insert_Base(root, node->equal, key + 1, len - 1, value);
    } else {
      assert(node->value == NULL);
      node->value = value;
    }
  } else {
    node->high = TriTree_Insert_Base(root, node->high, key, len, value);
  }

  return node;
}

TriTree *TriTree_Insert(TriTree *node, const char *key, size_t len,
                        void *value) {
  return TriTree_Insert_Base(node, node, key, len, value);
}

void *TriTree_Search(TriTree *root, const char *key, size_t len) {
  TriTree *node = root;
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

void *TriTree_Search_Prefix(TriTree *root, const char *key, size_t len) {
  if (len <= 0)
    return NULL;

  TriTree *node = root;
  TriTree *last = NULL;
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

void TriTree_Traverse(TriTree *node, TriTree_Traverse_Func func, void *data) {
  if (!node)
    return;

  if (node->low)
    TriTree_Traverse(node->low, func, data);

  if (node->equal)
    TriTree_Traverse(node->equal, func, data);

  if (node->high)
    TriTree_Traverse(node->high, func, data);

  if (node->value)
    func(node->value, data);
}

void TriTree_Destroy(TriTree *node) {
  if (node == NULL)
    return;

  if (node->low)
    TriTree_Destroy(node->low);

  if (node->equal)
    TriTree_Destroy(node->low);

  if (node->high)
    TriTree_Destroy(node->high);

  free(node);
}
