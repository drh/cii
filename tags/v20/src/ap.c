static const char *rcsid = "$Id$";
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
#define iszero(x) ((x)->ndigits==1 && (x)->digits[0]==0)
#define maxdigits(x,y) ((x)->ndigits > (y)->ndigits ? \
	(x)->ndigits : (y)->ndigits)
#define isone(x) ((x)->ndigits==1 && (x)->digits[0]==1)
static T normalize(T z, int n);
static int cmp(T x, T y);
static T mk(int size) {
	T z = CALLOC(1, sizeof (*z) + size);
	assert(size > 0);
	z->sign = 1;
	z->size = size;
	z->ndigits = 1;
	z->digits = (XP_T)(z + 1);
	return z;
}
static T set(T z, long int n) {
	if (n == LONG_MIN)
		XP_fromint(z->size, z->digits, LONG_MAX + 1UL);
	else if (n < 0)
		XP_fromint(z->size, z->digits, -n);
	else
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
		int carry = XP_add(n, z->digits, x->digits,
			y->digits, 0);
		z->digits[z->size-1] = XP_sum(x->ndigits - n,
			&z->digits[n], &x->digits[n], carry);
	} else
		z->digits[n] = XP_add(n, z->digits, x->digits,
			y->digits, 0);
	return normalize(z, z->size);
}
static T sub(T z, T x, T y) {
	int borrow, n = y->ndigits;
	borrow = XP_sub(n, z->digits, x->digits,
		y->digits, 0);
	if (x->ndigits > n)
		borrow = XP_diff(x->ndigits - n, &z->digits[n],
			&x->digits[n], borrow);
	assert(borrow == 0);
	return normalize(z, z->size);
}
static T mulmod(T x, T y, T p) {
	T z, xy = AP_mul(x, y);
	z = AP_mod(xy, p);
	AP_free(&xy);
	return z;
}
static int cmp(T x, T y) {
	if (x->ndigits != y->ndigits)
		return x->ndigits - y->ndigits;
	else
		return XP_cmp(x->ndigits, x->digits, y->digits);
}
T AP_new(long int n) {
	return set(mk(sizeof (long int)), n);
}
void AP_free(T *z) {
	assert(z && *z);
	FREE(*z);
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
	XP_mul(z->digits, x->ndigits, x->digits, y->ndigits,
		y->digits);
	normalize(z, z->size);
	z->sign = iszero(z)
		|| ((x->sign^y->sign) == 0) ? 1 : -1;
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
		XP_div(x->ndigits, q->digits, x->digits,
			y->ndigits, y->digits, r->digits, tmp);
		FREE(tmp);
	}
	normalize(q, q->size);
	normalize(r, r->size);
	q->sign = iszero(q)
		|| ((x->sign^y->sign) == 0) ? 1 : -1;
	if (!((x->sign^y->sign) == 0) && !iszero(r)) {
		int carry = XP_sum(q->size, q->digits,
			q->digits, 1);
		assert(carry == 0);
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
		XP_div(x->ndigits, q->digits, x->digits,
			y->ndigits, y->digits, r->digits, tmp);
		FREE(tmp);
	}
	normalize(q, q->size);
	normalize(r, r->size);
	q->sign = iszero(q)
		|| ((x->sign^y->sign) == 0) ? 1 : -1;
	if (!((x->sign^y->sign) == 0) && !iszero(r)) {
		int borrow = XP_sub(r->size, r->digits,
			y->digits, r->digits, 0);
		assert(borrow == 0);
		normalize(r, r->size);
	}
	AP_free(&q);
	return r;
}
T AP_pow(T x, T y, T p) {
	T z;
	assert(x);
	assert(y);
	assert(y->sign == 1);
	assert(!p || p->sign==1 && !iszero(p) && !isone(p));
	if (iszero(x))
		return AP_new(0);
	if (iszero(y))
		return AP_new(1);
	if (isone(x))
		return AP_new((((y)->digits[0]&1) == 0) ? 1 : x->sign);
	if (p)
		if (isone(y))
			z = AP_mod(x, p);
		else {
			T y2 = AP_rshift(y, 1), t = AP_pow(x, y2, p);
			z = mulmod(t, t, p);
			AP_free(&y2);
			AP_free(&t);
			if (!(((y)->digits[0]&1) == 0)) {
				z = mulmod(y2 = AP_mod(x, p), t = z, p);
				AP_free(&y2);
				AP_free(&t);
			}
		}
	else
		if (isone(y))
			z = AP_addi(x, 0);
		else {
			T y2 = AP_rshift(y, 1), t = AP_pow(x, y2, NULL);
			z = AP_mul(t, t);
			AP_free(&y2);
			AP_free(&t);
			if (!(((y)->digits[0]&1) == 0)) {
				z = AP_mul(x, t = z);
				AP_free(&t);
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
	XP_lshift(z->size, z->digits, x->ndigits,
		x->digits, s, 0);
	z->sign = x->sign;
	return normalize(z, z->size);
}
T AP_rshift(T x, int s) {
	assert(x);
	assert(s >= 0);
	if (s >= 8*x->ndigits)
		return AP_new(0);
	else {
		T z = mk(x->ndigits - s/8);
		XP_rshift(z->size, z->digits, x->ndigits,
			x->digits, s, 0);
		normalize(z, z->size);
		z->sign = iszero(z) ? 1 : x->sign;
		return z;
	}
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
	char *endp, sign = '\0';
	int carry;
	assert(p);
	assert(base >= 2 && base <= 36);
	while (*p && isspace(*p))
		p++;
	if (*p == '-' || *p == '+')
		sign = *p++;
	{
		const char *start;
		int k, n = 0;
		for ( ; *p == '0' && p[1] == '0'; p++)
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
	carry = XP_fromstr(z->size, z->digits, p,
		base, &endp);
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
	assert(base >= 2 && base <= 36);
	assert(str == NULL || size > 1);
	if (str == NULL) {
		{
			int k;
			for (k = 5; (1<<k) > base; k--)
				;
			size = (8*x->ndigits)/k + 1 + 1;
			if (x->sign == -1)
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
	FREE(q);
	return str;
}
void AP_fmt(int code, va_list_box *box,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision) {
	T x;
	char *buf;
	assert(box && flags);
	x = va_arg(box->ap, T);
	assert(x);
	buf = AP_tostr(NULL, 0, 10, x);
	Fmt_putd(buf, strlen(buf), put, cl, flags,
		width, precision);
	FREE(buf);
}
