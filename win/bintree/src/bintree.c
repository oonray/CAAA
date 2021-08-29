#include "bintree.h"

static int default_compare (void *a, void *b){return bstrcmp((bstring) a,(bstring) b);}

BinTree *BinTree_Create(BinTree_Compare compare){
	BinTree *map = calloc(1,sizeof(BinTree));
	check_memory(map);

	map->compare = compare == NULL ? default_compare : compare;
	return map;
error:
	if(map) BinTree_Destroy(map);
	return NULL;
}

static int BinTree_Destroy_CB(BinTreeNode *node){
	free(node);
	return 0;
}

void BinTree_Destroy(BinTree *bin){
	if(bin){
		BinTree_Traverse(bin,BinTree_Destroy_CB);
		free(bin);
	}
}

static inline BinTreeNode *BinTreeNode_Create(BinTreeNode *parent,void *key,void *data){
	BinTreeNode *node = calloc(1,sizeof(BinTreeNode));
	check_memory(node);

	node->key = key;
	node->data = data;
	node->parent = parent;
	return node;

error:
	return NULL;
}
static inline void BinTree_SetNode(BinTree *map,BinTreeNode *node,void *key,void *data){
	int cmp = map->compare(node->key,key);
	if(cmp <= 0){
			if(node->left){
					BinTree_SetNode(map,node->left,key,data);
			}else{
					node->left = BinTreeNode_Create(node,key,data);
			}
	}else{
			if(node->right){
					BinTree_SetNode(map,node->right, key, data);
			}else{
					node->right = BinTreeNode_Create(node,key,data);
			}
	}
}

int BinTree_Set(BinTree *map,void *key,void *data){
	if(map->root==NULL){
		map->root=BinTreeNode_Create(NULL,key,data);
		check_memory(map->root);
	}else{
		BinTree_SetNode(map,map->root,key,data);
	}
	return 0;
error:
	return -1;
}

static inline BinTreeNode *BinTreeNode_GetNode(BinTree *map, BinTreeNode *node,void *key){
		int cmp = map->compare(node->key,key);
		if(cmp == 0){
			return node;
		}else if(cmp < 0){
			if(node->left){
				return BinTreeNode_GetNode(map,node->left,key);
			}else{
				return NULL;
			}
		}else{
			if(node->right){
				return BinTreeNode_GetNode(map,node->right,key);
			}else{
				return NULL;
			}
		}
}

void *BinTree_Get(BinTree *map,void *key){
	if(map->root == NULL){
		return NULL;
	} else {
		BinTreeNode *node = BinTreeNode_GetNode(map,map->root,key);
		return node == NULL?NULL: node->data;
	}
}

static inline int BinTree_Traverse_Nodes(BinTreeNode *node,BinTree_traverse_cb traverse_cb){
	int rc = 0;	
	if(node->left){
		rc = BinTree_Traverse_Nodes(node->left,traverse_cb);
		if(rc != 0){
			return rc;
		}
	}
	if(node->right){
		rc = BinTree_Traverse_Nodes(node->right,traverse_cb);
		if(rc != 0){
			return rc;
		}
	}

	return traverse_cb(node);
} 

int BinTree_Traverse(BinTree *map,BinTree_traverse_cb traverse_cb){
	if(map->root){
		return BinTree_Traverse_Nodes(map->root,traverse_cb);
	}
	return 0;
}

static inline BinTreeNode *BinTree_find_min(BinTreeNode *node){
	while(node->left){
		node=node->left;
	}
	return node;
}

static inline void BinTree_Replace_Node_In_Parent(BinTree *map, BinTreeNode *node,BinTreeNode *new_value){
  if(node->parent){
		if(node == node->parent->left){
				node->parent->left = new_value;
		} else {
				node->parent->right = new_value;
		}
	} else {
		map->root = new_value;
	}		

	if(new_value){
		new_value->parent = node->parent;
	}
}

static inline void BinTree_Swap(BinTreeNode *a, BinTreeNode *b){
	void *tmp = NULL;
	tmp = b->key;
	
	b->key = a->key;
	a->key = tmp;

	tmp = b->data;
	b->data = a->data;
	a->data = tmp;
}

static inline BinTreeNode *BinTreeNode_delete(BinTree *map,BinTreeNode *node,void *key){
	 int cmp = map->compare(node->key,key);
	 if(cmp < 0){
			if(node->left){
					return BinTreeNode_delete(map,node->left,key);
			}else{
				return NULL;
			}
	 } else if(cmp > 0){
			if(node->right){
				return BinTreeNode_delete(map,node->right,key);
			}else{
				return NULL;
			}
	 }else{
			if(node->left && node->right){
				BinTreeNode *successor = BinTree_find_min(node->right);
				BinTree_Swap(successor,node);
				BinTree_Replace_Node_In_Parent(map,successor,successor->right);
				return successor;
			}else if(node->left){
				BinTree_Replace_Node_In_Parent(map,node,node->left);
			}else if(node->right){
				BinTree_Replace_Node_In_Parent(map,node,node->right);
			}else {
				BinTree_Replace_Node_In_Parent(map,node,NULL);
			}

			return node;
	 }
}

void *BinTree_Delete(BinTree *map,void *key){
		void *data = NULL;
		if(map->root){
			BinTreeNode *node = BinTreeNode_delete(map,map->root,key);
			if(node){
				data = node->data;
				free(node);
			}
		}

		return data;
}


