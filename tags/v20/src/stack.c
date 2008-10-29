static char rcsid[] = "$Id$";
#include <stddef.h>
#include "assert.h"
#include "mem.h"
#include "stack.h"
#define T Stack_T
struct T {
	int count;
	struct elem {
		void *x;
		struct elem *link;
	} *head;
};
T Stack_new(void) {
	T stk;
	NEW(stk);
	stk->count = 0;
	stk->head = NULL;
	return stk;
}
int Stack_empty(T stk) {
	assert(stk);
	return stk->count == 0;
}
void Stack_push(T stk, void *x) {
	struct elem *t;
	assert(stk);
	NEW(t);
	t->x = x;
	t->link = stk->head;
	stk->head = t;
	stk->count++;
}
void *Stack_pop(T stk) {
	void *x;
	struct elem *t;
	assert(stk);
	assert(stk->count > 0);
	t = stk->head;
	stk->head = t->link;
	stk->count--;
	x = t->x;
	FREE(t);
	return x;
}
void Stack_free(T *stk) {
	struct elem *t, *u;
	assert(stk && *stk);
	for (t = (*stk)->head; t; t = u) {
		u = t->link;
		FREE(t);
	}
	FREE(*stk);
}
