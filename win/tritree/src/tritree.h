#ifndef _tritree_h
#define _tritree_h

#include  <stdlib.h>
#include  <bstrlib.h>

typedef struct TriTree {
		char splitchar;
		struct TriTree *low;
		struct TriTree *equal;
		struct TriTree *high;
		void *value;
} TriTree;

typedef void (*TriTree_Traverse_Func) (void *value,void *data);

void *TriTree_Search(TriTree *root,const char *key,size_t len);
void *TriTree_Search_Prefix(TriTree *root,const char *key,size_t len);
TriTree *TriTree_Insert(TriTree *node,const char *key,size_t len,void *value);
void TriTree_Traverse(TriTree *node,TriTree_Traverse_Func func,void *data);
void TriTree_Destroy(TriTree *root);
#endif
