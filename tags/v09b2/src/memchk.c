#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "except.h"
#include "mem.h"
union align {
	int i;
	long l;
	unsigned u;
	void *p;
	void (*fp)(void);
	float f;
	double d;
};
#define hash(p,t) \
	(((unsigned)(p)>>3)&(sizeof (t)/sizeof ((t)[0])-1))
#define NDESCRIPTORS 512
#define NALLOC 4096
const Except_T Mem_Failed = { "Allocation Failed" };
static struct block {
	struct block *free;
	struct block *link;
	const void *ptr;
	int size;
	const char *file;
	int line;
} *htab[2048];
static struct block freelist = { &freelist };
static struct block *find(const void *ptr) {
	struct block *bp = htab[hash(ptr, htab)];
	while (bp != NULL && bp->ptr != ptr)
		bp = bp->link;
	return bp;
}
void Mem_free(void **ptr, const char *file, int line) {
	assert(ptr);
	if (*ptr != NULL) {
		struct block *bp;
		if (((unsigned long)ptr&(sizeof (union align) - 1)) != 0
		|| (bp = find(ptr)) == NULL || bp->free != NULL)
			Except_raise(&Assert_Failed, file, line);
		bp->free = freelist.free;
		freelist.free = bp;
	}
	*ptr = NULL;
}
void *Mem_resize(void **ptr, int nbytes,
	const char *file, int line) {
	struct block *bp;
	void *newptr;
	assert(ptr && *ptr);
	assert(nbytes > 0);
	if (((unsigned long)ptr&(sizeof (union align) - 1)) != 0
	|| (bp = find(ptr)) == NULL || bp->free != NULL)
		Except_raise(&Assert_Failed, file, line);
	newptr = Mem_alloc(nbytes, file, line);
	memcpy(newptr, *ptr, nbytes < bp->size ? nbytes : bp->size);
	Mem_free(ptr, file, line);
	return newptr;
}
void *Mem_calloc(int count, int nbytes,
	const char *file, int line) {
	void *ptr;
	assert(count > 0);
	assert(nbytes > 0);
	ptr = Mem_alloc(count*nbytes, file, line);
	memset(ptr, '\0', count*nbytes);
	return ptr;
}
static struct block *block(void *ptr, int size,
	const char *file, int line) {
	static struct block *avail;
	static int nleft;
	if (nleft <= 0) {
		avail = malloc(NDESCRIPTORS*sizeof (*avail));
		if (avail == NULL)
			return NULL;
		nleft = NDESCRIPTORS;
	}
	avail->ptr  = ptr;
	avail->size = size;
	avail->file = file;
	avail->line = line;
	avail->free = avail->link = NULL;
	nleft--;
	return avail++;
}
void *Mem_alloc(int nbytes, const char *file, int line) {
	struct block *bp;
	void *ptr;
	assert(nbytes > 0);
	nbytes = (nbytes +
		sizeof (union align) - 1)&~(sizeof (union align) - 1);
	for (bp = freelist.free; bp != NULL; bp = bp->free) {
		if (bp->size > nbytes) {
			bp->size -= nbytes;
			ptr = (char *)bp->ptr + bp->size;
			if ((bp = block(ptr, nbytes, file, line)) != NULL) {
				unsigned h = hash(ptr, htab);
				bp->link = htab[h];
				htab[h] = bp;
				return ptr;
			} else
				{
					if (file == NULL)
						RAISE(Mem_Failed);
					else
						Except_raise(&Mem_Failed, file, line);
				}
		}
		if (bp == &freelist) {
			struct block *newptr;
			if ((ptr = malloc(nbytes + NALLOC)) == NULL
			||  (newptr = block(ptr, nbytes + NALLOC,
					__FILE__, __LINE__)) == NULL)
				{
					if (file == NULL)
						RAISE(Mem_Failed);
					else
						Except_raise(&Mem_Failed, file, line);
				}
			newptr->free = freelist.free;
			freelist.free = newptr;
		}
	}
	assert(0);
	return NULL;
}
static char rcsid[] = "$RCSfile: RCS/mem.doc,v $ $Revision: 1.2 $";
