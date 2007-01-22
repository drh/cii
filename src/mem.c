#include <stdlib.h>
#include "assert.h"
#include "except.h"
#include "mem.h"
const Except_T Mem_Failed = { "Allocation Failed" };
void *Mem_alloc(int nbytes, const char *file, int line) {
	void *ptr;
	assert(nbytes > 0);
	ptr = malloc(nbytes);
	if (ptr == NULL)
		{
			if (file == NULL)
				RAISE(Mem_Failed);
			else
				Except_raise(&Mem_Failed, file, line);
		}
	return ptr;
}
void *Mem_calloc(int count, int nbytes,
	const char *file, int line) {
	void *ptr;
	assert(count > 0);
	assert(nbytes > 0);
	ptr = calloc(count, nbytes);
	if (ptr == NULL)
		{
			if (file == NULL)
				RAISE(Mem_Failed);
			else
				Except_raise(&Mem_Failed, file, line);
		}
	return ptr;
}
void Mem_free(void **ptr, const char *file, int line) {
	assert(ptr);
	free(*ptr);
	*ptr = NULL;
}
void *Mem_resize(void **ptr, int nbytes,
	const char *file, int line) {
	void *newptr;
	assert(ptr && *ptr);
	assert(nbytes > 0);
	newptr = realloc(*ptr, nbytes);
	if (newptr == NULL)
		{
			if (file == NULL)
				RAISE(Mem_Failed);
			else
				Except_raise(&Mem_Failed, file, line);
		}
	*ptr = NULL;
	return newptr;
}
static char rcsid[] = "$RCSfile: RCS/mem.doc,v $ $Revision: 1.6 $";
