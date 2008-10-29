#ifndef INTEGER_INCLUDED
#define INTEGER_INCLUDED

#define T Integer_T
typedef int *T;

extern T Integer_new(int n);
extern int Integer_get(T integer);
extern int Integer_put(T integer, int n);
extern int Integer_cmp(T i1, T i2);
extern void Integer_fmt(int code, va_list_box *box,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision);

#undef T
#endif
