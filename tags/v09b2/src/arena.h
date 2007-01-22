#ifndef ARENA_INCLUDED
#define ARENA_INCLUDED
#include "except.h"
#define T Arena_T
typedef struct T *T;
extern const Except_T Arena_NewFailed;
extern const Except_T Arena_Failed;
extern T Arena_new(void);
extern void Arena_dispose(T *ap);
extern void *Arena_alloc (T arena, int nbytes,
				const char *file, int line);
extern void *Arena_calloc(T arena, int count, int nbytes,
				const char *file, int line);
extern void  Arena_free(T arena);
#undef T
#endif
/* $RCSfile: RCS/arena.doc,v $ $Revision: 1.2 $ */
