#include "vector.h"

VectorNode *Vector_Node_New(void *data){
	VectorNode *node = (VectorNode *)calloc(1,sizeof(VectorNode));
	check_memory(node);
	node->data = data;
	return node;
error:
	if(node){
		free(node);
	}
	return NULL;
}

Vector *Vector_New(){
		Vector *arr = (Vector *)calloc(1,sizeof(Vector));
		check_memory(arr);
		arr->size = 0;
		return arr;
error:
		if(arr){
			free(arr);
		}
		return NULL;
} 

void Vector_Destroy(Vector *arr){
		if(arr){
			free(arr);
		}
}

int Vector_Add(Vector *v,void *data){
	VectorNode *node = Vector_Node_New(data);
	check_memory(node);

	if(v->first != NULL){
		VectorNode *last = Vector_Get_Last(v);
		check(last!=NULL,"Could not get last");

		last->next = node;
		++v->size;

	}else{
		v->first = node;
		++v->size;
	}

	return v->size;
error:
	free(node);
	return NULL;
}

int Vector_Add_Node(Vector *v,VectorNode *node){
	if(v->first != NULL){
		VectorNode *last = Vector_Get_Last(v);
		check(last!=NULL,"Could not get last");

		last->next = node;
		++v->size;

	}else{
		v->first = node;
		++v->size;
	}

	return v->size;
error:
	return 0;
}

int Vector_Set(Vector *v,int n, void *data){
}

void* Node_Get(VectorNode *n, int t){
	if(t == 0){return n->data;}
	else{return Node_Get(n->next, --t);}
}

void *Vector_Get(Vector *v, int n){
	check(n < v->size,"Out of Bounds");
	void *data = Node_Get(v->first,n);
	check(data!=NULL,"No Data Retrieved");
	return data;
error:
	return NULL;
}

void *Vector_Get_Last(Vector *v){
	VectorNode *node = v->first;
	check(node!=NULL,"No items in vector");

	while(node->next != NULL){
		node = node->next;
	}

	return node;
error:
	return NULL;
}

void *Vector_Del(Vector *v,int n){
	VectorNode *node = v->first;
	check(node!=NULL,"Node not in Vector");

	VectorNode *delnode = NULL;

	if(n==0){
		if(node->next=NULL){v->first = NULL;
		}else{v->first=node->next;}
		void *data = node->data;
		free(node);
		return data;
	}

	while(n>1){
		node = node->next;
		--n;
	}

	delnode = node->next;
	if(delnode->next == NULL){
		node->next=NULL;
	}else{
		node->next=delnode->next;
	}

	void *data = delnode->data;
	free(delnode);
	v->size--;
	return data;
error:
	return NULL;
}

Vector *Vector_Traberse(Vector *v,Vector_Traverse_Func *f){
	Vector *out = Vector_New();


}
