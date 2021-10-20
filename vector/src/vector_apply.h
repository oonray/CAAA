#ifndef VECTOR_APPLY_H_
#define VECTOR_APPLY_H_
#include <vector.h>

Vector * Vector_apply(Vector *v, Vector_Traverse_Func_Recurse func);

#endif // VECTOR_APPLY_H_


/*
** Vector *name = {a,b,c,d,e}
** (func (a,b) sum(a,b))
**
**
** */
