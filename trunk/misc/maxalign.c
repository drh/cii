#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
/*
On most platforms, malloc returns pointers to blocks that are
aligned on addresses that are multiples of the size of the largest
basic data type. Some CII functions use a union to determine this
multiple (cf. union align on p. 80). Alignments are less restrictive
on some platforms and, for these, MAXALIGN must be defined as the
alignment required.

This program attempts to determine the correct value for MAXALIGN, if
one is necessary, and echo the appropriate -D option. Unfortunately,
the method used relies on the C compiler using the same alignments as
malloc, which is not required. malloc is the final authority: If it
returns addresses that are multiples of sizeof (union align), then
MAXALIGN is unnecessary; otherwise, MAXALIGN must provide the
alignment. Incorrect values of MAXALIGN can cause crashes and
assertion failures.
*/
union align {
	int i;
	long l;
	long *lp;
	void *p;
	void (*fp)(void);
	float f;
	double d;
	long double ld;
};

typedef void (*functp);
#define yy \
	xx(int,i);\
	xx(long,l);\
	xx(long *,lp);\
	xx(void *,p);\
	xx(functp,fp);\
	xx(float,f);\
	xx(double,d);\
	xx(long double,ld);

int main(int argc, char *argv[]) {
#define xx(t,v)	struct { char pad; t v; } v
	yy
#undef xx
	unsigned max = 0;

#define xx(t,v) if ((char *)&v.v - &v.pad > max) max = (char *)&v.v - &v.pad
	yy
#undef yy
	if (argc > 1 && strcmp(argv[1], "-v") == 0)
		fprintf(stderr, "sizeof (union align) = %u\n", sizeof (union align));
	assert(max);
	if (max != sizeof (union align))
		printf("-DMAXALIGN=%u\n", max);
	return EXIT_SUCCESS;
}


