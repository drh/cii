#ifndef EXCEPT_INCLUDED
#define EXCEPT_INCLUDED
#include <setjmp.h>
#define T Except_T
typedef struct T {
	char *reason;
} T;
typedef struct Except_Frame Except_Frame;
struct Except_Frame {
	Except_Frame *prev;
	jmp_buf env;
	const char *file;
	int line;
	const T *exception;
};
extern Except_Frame *Except_stack;
void Except_raise(const T *e, const char *file, int line);
#define RAISE(e) Except_raise(&(e),__FILE__,(int)__LINE__)
#define RERAISE Except_raise(Except_frame.exception, \
	Except_frame.file, Except_frame.line)
#define RETURN switch (Except_stack = Except_stack->prev,0) default: return
#define TRY do { \
	int Except_flag; \
	Except_Frame Except_frame; \
	Except_frame.prev = Except_stack; \
	Except_stack = &Except_frame;  \
	if ((Except_flag = setjmp(Except_frame.env)) == 0) {
#define EXCEPT(e) \
		if (Except_flag == 0) Except_stack = Except_stack->prev; \
	} else if (Except_frame.exception == &(e)) { \
		Except_flag = 2;
#define ELSE \
		if (Except_flag == 0) Except_stack = Except_stack->prev; \
	} else { \
		Except_flag = 2;
#define FINALLY \
		if (Except_flag == 0) Except_stack = Except_stack->prev; \
	} { \
		if (Except_flag == 0) Except_flag = 3;
#define END_TRY \
		if (Except_flag == 0) Except_stack = Except_stack->prev; \
	} if (Except_flag == 1) RERAISE; } while (0)
#undef T
#endif
/* $RCSfile: RCS/except.doc,v $ $Revision: 1.2 $ */
