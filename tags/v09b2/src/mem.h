#ifndef MEM_INCLUDED
#define MEM_INCLUDED
#include "except.h"
extern const Except_T Mem_Failed;
extern void *Mem_alloc (int nbytes, const char *file, int line);
extern void *Mem_calloc(int count, int nbytes,
	const char *file, int line);
extern void Mem_free(void **ptr, const char *file, int line);
extern void *Mem_resize(void **ptr, int nbytes,
	const char *file, int line);
#define ALLOC(nbytes) Mem_alloc((nbytes), __FILE__, __LINE__)
#define CALLOC(count,nbytes) \
	Mem_calloc((count), (nbytes), __FILE__, __LINE__)
#define  NEW(p) ((p) = ALLOC((int)sizeof *(p)))
#define NEW0(p) ((p) = CALLOC(1, (int)sizeof *(p)))
#define FREE(ptr) Mem_free((void **)(ptr), __FILE__, __LINE__)
#define RESIZE(ptr,nbytes) \
	Mem_resize((void **)(ptr), (nbytes), __FILE__, __LINE__)
#endif
/* $RCSfile: RCS/mem.doc,v $ $Revision: 1.2 $ */
