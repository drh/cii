#ifndef XP_INCLUDED
#define XP_INCLUDED
#define T XP_T
typedef unsigned char *T;
extern unsigned XP_add(int n, T z, T x, T y, unsigned carry);
extern unsigned XP_sub(int n, T z, T x, T y, unsigned borrow);
extern unsigned XP_mul(int n, T z, T x, int m, T y);
extern int XP_div(int n, T q, T r, T x, int m, T y, T tmp);
extern unsigned XP_sum     (int n, T z, T x, unsigned y);
extern unsigned XP_diff    (int n, T z, T x, unsigned y);
extern unsigned XP_product (int n, T z, T x, unsigned y);
extern unsigned XP_quotient(int n, T z, T x, unsigned y);
extern unsigned XP_neg(int n, T z, T x, unsigned carry);
extern int XP_cmp(int n, T x, T y);
extern void XP_lshift(int n, T z, int m, T x,
	int s, unsigned fill);
extern void XP_rshift(int n, T z, int m, T x,
	int s, unsigned fill);
extern int XP_length(int n, T x);
extern unsigned long XP_fromint(int n, T z, unsigned long u);
extern unsigned long XP_toint  (int n, T x);
extern unsigned XP_fromstr(int n, T z, const char *str,
	int base, char **end);
extern char *XP_tostr(char *str, int size, int base,
	int n, T x);
#undef T
#endif
/* $RCSfile: RCS/xp.doc,v $ $Revision: 1.2 $ */
