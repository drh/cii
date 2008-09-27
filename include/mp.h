/* $Id$ */
#ifndef MP_INCLUDED
#define MP_INCLUDED
#include <stdarg.h>
#include <stddef.h>
#include "except.h"
#include "fmt.h"
#define T MP_T
typedef unsigned char *T;
extern const Except_T MP_Overflow;
extern const Except_T MP_Dividebyzero;
extern int MP_set(int n);
extern T MP_new(unsigned long u);
extern T MP_fromint (T z, long v);
extern T MP_fromintu(T z, unsigned long u);
extern unsigned long MP_tointu(T x);
extern          long MP_toint (T x);
extern T MP_cvt (int m, T z, T x);
extern T MP_cvtu(int m, T z, T x);
extern T MP_add (T z, T x, T y);
extern T MP_sub (T z, T x, T y);
extern T MP_mul (T z, T x, T y);
extern T MP_div (T z, T x, T y);
extern T MP_mod (T z, T x, T y);
extern T MP_neg (T z, T x);
extern T MP_addu(T z, T x, T y);
extern T MP_subu(T z, T x, T y);
extern T MP_mulu(T z, T x, T y);
extern T MP_divu(T z, T x, T y);
extern T MP_modu(T z, T x, T y);
extern T MP_mul2u(T z, T x, T y);
extern T MP_mul2 (T z, T x, T y);
extern T MP_addi (T z, T x, long y);
extern T MP_subi (T z, T x, long y);
extern T MP_muli (T z, T x, long y);
extern T MP_divi (T z, T x, long y);
extern T MP_addui(T z, T x, unsigned long y);
extern T MP_subui(T z, T x, unsigned long y);
extern T MP_mului(T z, T x, unsigned long y);
extern T MP_divui(T z, T x, unsigned long y);
extern          long MP_modi (T x,          long y);
extern unsigned long MP_modui(T x, unsigned long y);
extern int MP_cmp  (T x, T y);
extern int MP_cmpi (T x, long y);
extern int MP_cmpu (T x, T y);
extern int MP_cmpui(T x, unsigned long y);
extern T MP_and (T z, T x, T y);
extern T MP_or  (T z, T x, T y);
extern T MP_xor (T z, T x, T y);
extern T MP_not (T z, T x);
extern T MP_andi(T z, T x, unsigned long y);
extern T MP_ori (T z, T x, unsigned long y);
extern T MP_xori(T z, T x, unsigned long y);
extern T MP_lshift(T z, T x, int s);
extern T MP_rshift(T z, T x, int s);
extern T MP_ashift(T z, T x, int s);
extern T     MP_fromstr(T z, const char *str,
	int base, char **end);
extern char *MP_tostr  (char *str, int size,
	int base, T x);
extern void  MP_fmt    (int code, va_list_box *box,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision);
extern void  MP_fmtu   (int code, va_list_box *box,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision);
#undef T
#endif
