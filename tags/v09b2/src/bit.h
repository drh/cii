#ifndef BIT_INCLUDED
#define BIT_INCLUDED
#define T Bit_T
typedef struct T *T;
extern T   Bit_new(int size);
extern int Bit_size  (T set);
extern int Bit_length(T set);
extern void Bit_free(T *set);
extern int  Bit_member(T set, int n);
extern void Bit_put   (T set, int n);
extern void Bit_remove(T set, int n);
extern void Bit_clear(T set, int lo, int hi);
extern void Bit_set  (T set, int lo, int hi);
extern void Bit_not  (T set, int lo, int hi);
extern int Bit_lt (T s, T t);
extern int Bit_eq (T s, T t);
extern int Bit_leq(T s, T t);
extern void Bit_map(T set,
 	void apply(int n, int bit, void *cl), void *cl);
extern T Bit_union(T s, T t);
extern T Bit_inter(T s, T t);
extern T Bit_minus(T s, T t);
extern T Bit_diff (T s, T t);
#undef T
#endif
/* $RCSfile: RCS/bit.doc,v $ $Revision: 1.3 $ */
