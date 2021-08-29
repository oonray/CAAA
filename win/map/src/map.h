#ifndef __map_h
#define __map_h

#include <stdint.h>
#include <c/dbg.h>
#include <c/vector.h>
#include "bstrlib.h"

#define DEFAULT_BUCKET_NUM 100

typedef int (*Map_Compare)(void *a, void *b);
typedef uint32_t (*Map_Hash)(void *key);


typedef struct Map {
	Vector *buckets;
	Map_Compare compare;
	Map_Hash hash;
} Map;

typedef struct Map_Node{
	void *key;
	void *data;
	uint32_t hash;
}Map_Node;

typedef int (*Map_Travese_Func)(Map_Node *node);

Map *Map_New(Map_Compare compare,Map_Hash hash);
void Map_Destroy(Map *m);

int Map_Set(Map *m,void *key,void *data);
void *Map_Get(Map *m,void *key);
void *Map_Delete(Map *m,void *key);

int Map_Travese(Map *m,Map_Travese_Func func);

static int default_compare(void *a, void *b);
static uint32_t default_hash(void *key);
#endif
