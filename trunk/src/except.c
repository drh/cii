static char rcsid[] = "$Id$" "\n$Id$";
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "except.h"
#define T Except_T
Except_Frame *Except_stack = NULL;
void Except_raise(const T *e, const char *file,
	int line) {
#ifdef WIN32
	Except_Frame *p;

	if (Except_index == -1)
		Except_init();
	p = TlsGetValue(Except_index);
#else
	Except_Frame *p = Except_stack;
#endif
	assert(e);
	if (p == NULL) {
		fprintf(stderr, "Uncaught exception");
		if (e->reason)
			fprintf(stderr, " %s", e->reason);
		else
			fprintf(stderr, " at 0x%p", e);
		if (file && line > 0)
			fprintf(stderr, " raised at %s:%d\n", file, line);
		fprintf(stderr, "aborting...\n");
		fflush(stderr);
		abort();
	}
	p->exception = e;
	p->file = file;
	p->line = line;
#ifdef WIN32
	Except_pop();
#else
	Except_stack = Except_stack->prev;
#endif
	longjmp(p->env, Except_raised);
}
#ifdef WIN32
_CRTIMP void __cdecl _assert(void *, void *, unsigned);
#undef assert
#define assert(e) ((e) || (_assert(#e, __FILE__, __LINE__), 0))

int Except_index = -1;
void Except_init(void) {
	BOOL cond;

	Except_index = TlsAlloc();
	assert(Except_index != TLS_OUT_OF_INDEXES);
	cond = TlsSetValue(Except_index, NULL);
	assert(cond == TRUE);
}

void Except_push(Except_Frame *fp) {
	BOOL cond;

	fp->prev = TlsGetValue(Except_index);
	cond = TlsSetValue(Except_index, fp);
	assert(cond == TRUE);
}

void Except_pop(void) {
	BOOL cond;
	Except_Frame *tos = TlsGetValue(Except_index);

	cond = TlsSetValue(Except_index, tos->prev);
	assert(cond == TRUE);
}
#endif
