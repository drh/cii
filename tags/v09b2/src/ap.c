#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "ap.h"
#include "fmt.h"
#include "xp.h"
#include "mem.h"
#define T AP_T
struct T {
	int sign;
	int ndigits;
	int size;
	XP_T digits;
};
#define iszero(x) ((x)->ndigits == 1 && (x)->digits[0] == 0)
#define maxdigits(x,y) ((x)->ndigits > (y)->ndigits ? \
	(x)->ndigits : (y)->ndigits)
#define isone(x) ((x)->ndigits == 1 && (x)->digits[0] == 1)
static T normalize(T z, int n);
static int cmp(T x, T y);
static T mk(int size) {
	T z = CALLOC(1, sizeof *z + size);
	z->sign = 1;
	z->size = size;
	z->ndigits = 1;
	z->digits = (void *)(z + 1);
	return z;
}
static T set(T z, long int n) {
	XP_fromint(z->size, z->digits, n);
	z->sign = n < 0 ? -1 : 1;
	return normalize(z, z->size);
}
static T normalize(T z, int n) {
	z->ndigits = XP_length(n, z->digits);
	return z;
}
static T add(T z, T x, T y) {
	int n = y->ndigits;
	if (x->ndigits < n)
		return add(z, y, x);
	else if (x->ndigits > n) {
		unsigned carry = XP_add(n, z->digits, x->digits,
			y->digits, 0);
		XP_sum(z->size - n, &z->digits[n], &x->digits[n],
			carry);
		return normalize(z, z->size);
	} else {
		z->digits[n] = XP_add(n, z->digits, x->digits,
			y->digits, 0);
		return normalize(z, n + 1);
	}
}
static T sub(T z, T x, T y) {
	int n = y->ndigits;
	unsigned borrow;
	borrow = XP_sub(n, z->digits, x->digits, y->digits, 0);
	if (x->ndigits > n) {
		XP_diff(z->size - n, &z->digits[n], &x->digits[n],
			borrow);
		return normalize(z, z->size);
	} else
		return normalize(z, n + 1);
}
static T mulmod(T x, T y, T p) {
	if (isone(y))
		return AP_mod(x, p);
	else {
		T z;
		x = AP_mod(x, p);
		y = AP_mod(y, p);
		z = AP_mul(x, y);
		AP_free(&x);
		AP_free(&y);
		return z;
	}
}
static int cmp(T x, T y) {
	if (x->ndigits != y->ndigits)
		return x->ndigits - y->ndigits;
	else
		return XP_cmp(x->ndigits, x->digits, y->digits);
}
T AP_new(long int n) {
	return set(mk(sizeof (unsigned long)), n);
}
void AP_free(T *z) {
	assert(z && *z);
	FREE(z);
}
T AP_neg(T x) {
	T z;
	assert(x);
	z = mk(x->ndigits);
	memcpy(z->digits, x->digits, x->ndigits);
	z->ndigits = x->ndigits;
	z->sign = iszero(z) ? 1 : -x->sign;
	return z;
}
T AP_mul(T x, T y) {
	T z;
	assert(x);
	assert(y);
	z = mk(x->ndigits + y->ndigits);
	XP_mul(x->ndigits, z->digits, x->digits, y->ndigits,
		y->digits);
	normalize(z, z->size);
	z->sign = iszero(z) || ((x->sign^y->sign) == 0) ? 1 : -1;
	return z;
}
T AP_add(T x, T y) {
	T z;
	assert(x);
	assert(y);
	if (((x->sign^y->sign) == 0)) {
		z = add(mk(maxdigits(x,y) + 1), x, y);
		z->sign = iszero(z) ? 1 : x->sign;
	} else
		if (cmp(x, y) > 0) {
			z = sub(mk(x->ndigits), x, y);
			z->sign = iszero(z) ? 1 : x->sign;
		}
		else {
			z = sub(mk(y->ndigits), y, x);
			z->sign = iszero(z) ? 1 : -x->sign;
		}
	return z;
}
T AP_sub(T x, T y) {
	T z;
	assert(x);
	assert(y);
	if (!((x->sign^y->sign) == 0)) {
		z = add(mk(maxdigits(x,y) + 1), x, y);
		z->sign = iszero(z) ? 1 : x->sign;
	} else
		if (cmp(x, y) > 0) {
			z = sub(mk(x->ndigits), x, y);
			z->sign = iszero(z) ? 1 : x->sign;
		} else {
			z = sub(mk(y->ndigits), y, x);
			z->sign = iszero(z) ? 1 : -x->sign;
		}
	return z;
}
T AP_div(T x, T y) {
	T q, r;
	assert(x);
	assert(y);
	assert(!iszero(y));
	q = mk(x->ndigits);
	r = mk(y->ndigits);
	{
		XP_T tmp = ALLOC(x->ndigits + y->ndigits + 2);
		XP_div(x->ndigits, q->digits, r->digits, x->digits,
			y->ndigits, y->digits, tmp);
		FREE(&tmp);
	}
	normalize(q, q->size);
	normalize(r, r->size);
	q->sign = iszero(q) || ((x->sign^y->sign) == 0) ? 1 : -1;
	if (!((x->sign^y->sign) == 0) && !iszero(r)) {
		XP_sum(q->size, q->digits, q->digits, 1);
		normalize(q, q->size);
	}
	AP_free(&r);
	return q;
}
T AP_mod(T x, T y) {
	T q, r;
	assert(x);
	assert(y);
	assert(!iszero(y));
	q = mk(x->ndigits);
	r = mk(y->ndigits);
	{
		XP_T tmp = ALLOC(x->ndigits + y->ndigits + 2);
		XP_div(x->ndigits, q->digits, r->digits, x->digits,
			y->ndigits, y->digits, tmp);
		FREE(&tmp);
	}
	normalize(q, q->size);
	normalize(r, r->size);
	q->sign = iszero(q) || ((x->sign^y->sign) == 0) ? 1 : -1;
	if (!((x->sign^y->sign) == 0) && !iszero(r)) {
		XP_sub(r->size, r->digits, y->digits, r->digits, 0);
		normalize(r, r->size);
	}
	AP_free(&q);
	return r;
}
T AP_pow(T x, T y, T p) {
	AP_T z;
	assert(x);
	assert(y);
	assert(y->sign == 1 && !iszero(y));
	if (p) {
		assert(p);
		assert(p->sign == 1 && !iszero(p) && !isone(p));
		if (iszero(x))
			return AP_new(0);
		if (iszero(y))
			return AP_new(1);
		if (isone(y) || isone(x))
			return AP_addi(x, 0);
		if ((((y)->digits[0]&1) == 0)) {
			y = AP_rshift(y, 1);
			x = AP_pow(x, y, p);
			AP_free(&y);
			z = mulmod(x, x, p);
			AP_free(&x);
		} else {
			T y1 = AP_subi(y, 1);
			y = AP_pow(x, y1, p);
			AP_free(&y1);
			z = mulmod(x, y, p);
			AP_free(&y);
		}
	} else {
		if (iszero(x))
			return AP_new(0);
		if (iszero(y))
			return AP_new(1);
		if (isone(y) || isone(x))
			return AP_addi(x, 0);
		if ((((y)->digits[0]&1) == 0)) {
			y = AP_rshift(y, 1);
			x = AP_pow(x, y, NULL);
			AP_free(&y);
			z = AP_mul(x, x);
			AP_free(&x);
		} else {
			AP_T y1 = AP_subi(y, 1);
			y = AP_pow(x, y1, NULL);
			AP_free(&y1);
			z = AP_mul(x, y);
			AP_free(&y);
		}
	}
	return z;
}
int AP_cmp(T x, T y) {
	assert(x);
	assert(y);
	if (!((x->sign^y->sign) == 0))
		return x->sign;
	else if (x->sign == 1)
		return cmp(x, y);
	else
		return cmp(y, x);
}
T AP_addi(T x, long int y) {
	unsigned char d[sizeof (unsigned long)];
	struct T t;
	t.size = sizeof d;
	t.digits = d;
	return AP_add(x, set(&t, y));
}
T AP_subi(T x, long int y) {
	unsigned char d[sizeof (unsigned long)];
	struct T t;
	t.size = sizeof d;
	t.digits = d;
	return AP_sub(x, set(&t, y));
}
T AP_muli(T x, long int y) {
	unsigned char d[sizeof (unsigned long)];
	struct T t;
	t.size = sizeof d;
	t.digits = d;
	return AP_mul(x, set(&t, y));
}
T AP_divi(T x, long int y) {
	unsigned char d[sizeof (unsigned long)];
	struct T t;
	t.size = sizeof d;
	t.digits = d;
	return AP_div(x, set(&t, y));
}
int AP_cmpi(T x, long int y) {
	unsigned char d[sizeof (unsigned long)];
	struct T t;
	t.size = sizeof d;
	t.digits = d;
	return AP_cmp(x, set(&t, y));
}
long int AP_modi(T x, long int y) {
	long int rem;
	T r;
	unsigned char d[sizeof (unsigned long)];
	struct T t;
	t.size = sizeof d;
	t.digits = d;
	r = AP_mod(x, set(&t, y));
	rem = XP_toint(r->ndigits, r->digits);
	AP_free(&r);
	return rem;
}
T AP_lshift(T x, int s) {
	T z;
	assert(x);
	assert(s >= 0);
	z = mk(x->ndigits + ((s+7)&~7)/8);
	z->sign = x->sign;
	XP_lshift(z->size, z->digits, x->ndigits, x->digits, s, 0);
	return normalize(z, z->size);
}
T AP_rshift(T x, int s) {
	T z;
	assert(x);
	assert(s >= 0);
	z = mk(x->ndigits + ((s+7)&~7)/8);
	z->sign = x->sign;
	XP_rshift(z->size, z->digits, x->ndigits, x->digits, s, 0);
	return normalize(z, z->size);
}
long int AP_toint(T x) {
	unsigned long u;
	assert(x);
	u = XP_toint(x->ndigits, x->digits)%(LONG_MAX + 1UL);
	if (x->sign == -1)
		return -(long)u;
	else
		return  (long)u;
}
T AP_fromstr(const char *str, int base, char **end) {
	T z;
	const char *p = str;
	char *endp, sign = 0;
	unsigned carry;
	assert(str);
	assert(base >= 2 && base <= 36);
	while (*p && isspace(*p))
		p++;
	if (*p == '-' || *p == '+')
		sign = *p++;
	{
		const char *start;
		int k, n = 0;
		for ( ; *p == '0'; p++)
			;
		start = p;
		for ( ; (  '0' <= *p && *p <= '9' && *p < '0' + base
			|| 'a' <= *p && *p <= 'z' && *p < 'a' + base - 10
			|| 'A' <= *p && *p <= 'Z' && *p < 'A' + base - 10); p++)
			n++;
		for (k = 1; (1<<k) < base; k++)
			;
		z = mk(((k*n + 7)&~7)/8);
		p = start;
	}
	carry = XP_fromstr(z->size, z->digits, p, base, &endp);
	assert(carry == 0);
	normalize(z, z->size);
	if (endp == p) {
		endp = (char *)str;
		z = AP_new(0);
	} else
		z->sign = iszero(z) || sign != '-' ? 1 : -1;
	if (end)
		*end = (char *)endp;
	return z;
}
char *AP_tostr(char *str, int size, int base, T x) {
	XP_T q;
	assert(x);
	assert(base > 1 && base <= 36);
	assert(str == NULL || size > 1);
	if (!str) {
		{
			int k;
			for (k = 2; (1<<k) < base; k++)
				;
			size = (8*x->ndigits)/(k-1) + 1 + 1;
			if (x->sign == 1)
				size++;
		}
		str = ALLOC(size);
	}
	q = ALLOC(x->ndigits);
	memcpy(q, x->digits, x->ndigits);
	if (x->sign == -1) {
		str[0] = '-';
		XP_tostr(str + 1, size - 1, base, x->ndigits, q);
	} else
		XP_tostr(str, size, base, x->ndigits, q);
	return str;
}
void AP_fmt(int code, va_list *app,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision) {
	T x = va_arg(*app, T);
	char *buf;
	assert(x);
	buf = AP_tostr(NULL, 0, 10, x);
	Fmt_putd(buf, strlen(buf), put, cl, flags,
		width, precision);
	FREE(&buf);
}
static char rcsid[] = "$RCSfile: RCS/ap.doc,v $ $Revision: 1.2 $";
