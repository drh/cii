#include "fmt.h"
#include "assert.h"
#include "mem.h"

#define T Integer_T
typedef int *T;

T Integer_new(int n) {
	T integer;

	NEW(integer);
	*integer = n;
	return integer;
}

int Integer_get(T integer) {
	assert(integer);
	return *integer;
}

int Integer_put(T integer, int n) {
	int prev;

	assert(integer);
	prev = *integer;
	*integer = n;
	return prev;
}

int Integer_cmp(T i1, T i2) {
	assert(i1);
	assert(i2);
	if (*i1 < *i2)
		return -1;
	else if (*i1 > *i2)
		return +1;
	else
		return 0;

}

void Integer_fmt(int code, va_list_box *box,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision) {
	char buf[44];
	T integer;

	assert(box && flags);
	integer = va_arg(box->ap, T);
	assert(integer);
 	Fmt_putd(buf, Fmt_sfmt(buf, sizeof buf, "%d", *integer),
		put, cl, flags, width, precision);
}
