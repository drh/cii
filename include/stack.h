/* $Id: H:/drh/idioms/book/RCS/inter.doc,v 1.11 1997/02/21 19:42:15 drh Exp $ */
#ifndef STACK_INCLUDED
#define STACK_INCLUDED
#define T Stack_T
typedef struct T *T;
extern T     Stack_new  (void);
extern int   Stack_empty(T stk);
extern void  Stack_push (T stk, void *x);
extern void *Stack_pop  (T stk);
extern void  Stack_free (T *stk);
#undef T
#endif
