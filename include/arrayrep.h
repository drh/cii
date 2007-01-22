/* $Id: H:/drh/idioms/book/RCS/array.doc,v 1.11 1997/10/29 22:05:21 drh Exp $ */
#ifndef ARRAYREP_INCLUDED
#define ARRAYREP_INCLUDED
#define T Array_T
struct T {
	int length;
	int size;
	char *array;
};
extern void ArrayRep_init(T array, int length,
	int size, void *ary);
#undef T
#endif
