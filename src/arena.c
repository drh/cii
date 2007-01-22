#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "except.h"
#include "arena.h"
#define T Arena_T
const Except_T Arena_NewFailed = { "Arena Creation Failed"   };
const Except_T Arena_Failed    = { "Arena Allocation Failed" };
#define THRESHOLD 10
struct T {
	T prev;
	char *avail;
	char *limit;
};
union align {
	int i;
	long l;
	unsigned u;
	void *p;
	void (*fp)(void);
	float f;
	double d;
	long double ld;
};
union header {
	struct T b;
	union align a;
};
static T freechunks;
static int nfree;
T Arena_new(void) {
	T arena = calloc(1, sizeof *arena);
	if (arena == NULL)
		RAISE(Arena_NewFailed);
	return arena;
}
void Arena_dispose(T *ap) {
	assert(ap && *ap);
	Arena_free(*ap);
	free(*ap);
	*ap = NULL;
}
void *Arena_alloc(T arena, int nbytes,
	const char *file, int line) {
	assert(arena);
	assert(nbytes > 0);
	nbytes = (nbytes +
		sizeof (union align) - 1)&~(sizeof (union align) - 1);
	while (arena->avail + nbytes > arena->limit) {
		T ptr;
		char *limit;
if ((ptr = freechunks) != NULL) {
	freechunks = freechunks->prev;
	nfree--;
	limit = ptr->limit;
} else {
	int m = sizeof (union header) + nbytes + 10*1024;
	ptr = malloc(m);
	if (ptr == NULL)
		{
			if (file == NULL)
				RAISE(Arena_Failed);
			else
				Except_raise(&Arena_Failed, file, line);
		}
	limit = (char *)ptr + m;
}
		*ptr = *arena;
		arena->avail = (char *)((union header *)ptr + 1);
		arena->limit = limit;
		arena->prev  = ptr;
	}
	arena->avail += nbytes;
	return arena->avail - nbytes;
}
void *Arena_calloc(T arena, int count, int nbytes,
	const char *file, int line) {
	void *ptr;
	assert(count > 0);
	ptr = Arena_alloc(arena, count*nbytes, file, line);
	memset(ptr, '\0', count*nbytes);
	return ptr;
}
void Arena_free(T arena) {
	while (arena->prev) {
		struct T tmp = *arena->prev;
		if (nfree < THRESHOLD) {
			arena->prev->prev = freechunks;
			freechunks = arena->prev;
			nfree++;
			freechunks->limit = arena->limit;
		} else
			free(arena->prev);
		*arena = tmp;
	}
	assert(arena->limit);
	assert(arena->avail);
}
static char rcsid[] = "$RCSfile: RCS/arena.doc,v $ $Revision: 1.2 $";
