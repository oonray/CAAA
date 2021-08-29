#ifndef _BinTree_h
#define _BinTree_h

#include <stdlib.h>
#include <c/dbg.h>
#include "bstrlib.h"


typedef int (*BinTree_Compare) (void *a, void *b);

typedef struct BinTreeNode{
	void *key;
	void *data;

	struct BinTreeNode *left;
	struct BinTreeNode *right;
	struct BinTreeNode *parent;
}BinTreeNode;

typedef struct BinTree {
	int count;
	BinTree_Compare compare;
	BinTreeNode *root;
} BinTree;

typedef int (*BinTree_traverse_cb)(BinTreeNode *node);

BinTree *BinTree_Create(BinTree_Compare compare);
void BinTree_Destroy(BinTree *bin);

int BinTree_Set(BinTree *bin,void *key,void *data);
void *BinTree_Get(BinTree *bin,void *key);

int BinTree_Traverse(BinTree *bin, BinTree_traverse_cb trav);
void *BinTree_Delete(BinTree *map,void *key);

#endif
