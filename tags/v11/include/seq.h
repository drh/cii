/* $Id: H:/drh/idioms/book/RCS/seq.doc,v 1.11 1997/02/21 19:48:24 drh Exp $ */
#ifndef SEQ_INCLUDED
#define SEQ_INCLUDED
#define T Seq_T
typedef struct T *T;
extern T Seq_new(int hint);
extern T Seq_seq(void *x, ...);
extern void Seq_free(T *seq);
extern int Seq_length(T seq);
extern void *Seq_get(T seq, int i);
extern void *Seq_put(T seq, int i, void *x);
extern void *Seq_addlo(T seq, void *x);
extern void *Seq_addhi(T seq, void *x);
extern void *Seq_remlo(T seq);
extern void *Seq_remhi(T seq);
#undef T
#endif
