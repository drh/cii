/* $Id$ */
#ifndef AP_INCLUDED
#define AP_INCLUDED
#include <stdarg.h>
#include "fmt.h"
#define T AP_T
typedef struct T *T;
extern T AP_new    (long int n);
extern T AP_fromstr(const char *str, int base,
	char **end);
extern long int AP_toint(T x);
extern char    *AP_tostr(char *str, int size,
	int base, T x);
extern void     AP_fmt(int code, va_list_box *box,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision);
extern void AP_free(T *z);
extern T AP_neg(T x);
extern T AP_add(T x, T y);
extern T AP_sub(T x, T y);
extern T AP_mul(T x, T y);
extern T AP_div(T x, T y);
extern T AP_mod(T x, T y);
extern T AP_pow(T x, T y, T p);
extern T    AP_addi(T x, long int y);
extern T    AP_subi(T x, long int y);
extern T    AP_muli(T x, long int y);
extern T    AP_divi(T x, long int y);
extern long AP_modi(T x, long int y);
extern T AP_lshift(T x, int s);
extern T AP_rshift(T x, int s);
extern int AP_cmp (T x, T y);
extern int AP_cmpi(T x, long int y);
#undef T
#endif
