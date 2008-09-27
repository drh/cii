static char rcsid[] = "$Id$";
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <ctype.h>
#include <math.h>
#include "assert.h"
#include "except.h"
#include "fmt.h"
#include "mem.h"
#define T Fmt_T
struct buf {
	char *buf;
	char *bp;
	int size;
};
#define pad(n,c) do { int nn = (n); \
	while (nn-- > 0) \
		put((c), cl); } while (0)
static void cvt_s(int code, va_list_box *box,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision) {
	char *str = va_arg(box->ap, char *);
	assert(str);
	Fmt_puts(str, strlen(str), put, cl, flags,
		width, precision);
}
static void cvt_d(int code, va_list_box *box,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision) {
	int val = va_arg(box->ap, int);
	unsigned m;
	char buf[43];
	char *p = buf + sizeof buf;
	if (val == INT_MIN)
		m = INT_MAX + 1U;
	else if (val < 0)
		m = -val;
	else
		m = val;
	do
		*--p = m%10 + '0';
	while ((m /= 10) > 0);
	if (val < 0)
		*--p = '-';
	Fmt_putd(p, (buf + sizeof buf) - p, put, cl, flags,
		width, precision);
}
static void cvt_u(int code, va_list_box *box,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision) {
	unsigned m = va_arg(box->ap, unsigned);
	char buf[43];
	char *p = buf + sizeof buf;
	do
		*--p = m%10 + '0';
	while ((m /= 10) > 0);
	Fmt_putd(p, (buf + sizeof buf) - p, put, cl, flags,
		width, precision);
}
static void cvt_o(int code, va_list_box *box,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision) {
	unsigned m = va_arg(box->ap, unsigned);
	char buf[43];
	char *p = buf + sizeof buf;
	do
		*--p = (m&0x7) + '0';
	while ((m>>= 3) != 0);
	Fmt_putd(p, (buf + sizeof buf) - p, put, cl, flags,
		width, precision);
}
static void cvt_x(int code, va_list_box *box,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision) {
	unsigned m = va_arg(box->ap, unsigned);
	char buf[43];
	char *p = buf + sizeof buf;
	do
		*--p = "0123456789abcdef"[m&0xf];
	while ((m>>= 4) != 0);
	Fmt_putd(p, (buf + sizeof buf) - p, put, cl, flags,
		width, precision);
}
static void cvt_p(int code, va_list_box *box,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision) {
	unsigned long m = (unsigned long)va_arg(box->ap, void*);
	char buf[43];
	char *p = buf + sizeof buf;
	precision = INT_MIN;
	do
		*--p = "0123456789abcdef"[m&0xf];
	while ((m>>= 4) != 0);
	Fmt_putd(p, (buf + sizeof buf) - p, put, cl, flags,
		width, precision);
}
static void cvt_c(int code, va_list_box *box,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision) {
	if (width == INT_MIN)
		width = 0;
	if (width < 0) {
		flags['-'] = 1;
		width = -width;
	}
	if (!flags['-'])
		pad(width - 1, ' ');
	put((unsigned char)va_arg(box->ap, int), cl);
	if ( flags['-'])
		pad(width - 1, ' ');
}
static void cvt_f(int code, va_list_box *box,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision) {
	char buf[DBL_MAX_10_EXP+1+1+99+1];
	if (precision < 0)
		precision = 6;
	if (code == 'g' && precision == 0)
		precision = 1;
	{
		static char fmt[] = "%.dd?";
		assert(precision <= 99);
		fmt[4] = code;
		fmt[3] =      precision%10 + '0';
		fmt[2] = (precision/10)%10 + '0';
		sprintf(buf, fmt, va_arg(box->ap, double));
	}
	Fmt_putd(buf, strlen(buf), put, cl, flags,
		width, precision);
}
const Except_T Fmt_Overflow = { "Formatting Overflow" };
static T cvt[256] = {
 /*   0-  7 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*   8- 15 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  16- 23 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  24- 31 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  32- 39 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  40- 47 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  48- 55 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  56- 63 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  64- 71 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  72- 79 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  80- 87 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  88- 95 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  96-103 */ 0,     0, 0, cvt_c, cvt_d, cvt_f, cvt_f, cvt_f,
 /* 104-111 */ 0,     0, 0,     0,     0,     0,     0, cvt_o,
 /* 112-119 */ cvt_p, 0, 0, cvt_s,     0, cvt_u,     0,     0,
 /* 120-127 */ cvt_x, 0, 0,     0,     0,     0,     0,     0
};
char *Fmt_flags = "-+ 0";
static int outc(int c, void *cl) {
	FILE *f = cl;
	return putc(c, f);
}
static int insert(int c, void *cl) {
	struct buf *p = cl;
	if (p->bp >= p->buf + p->size)
		RAISE(Fmt_Overflow);
	*p->bp++ = c;
	return c;
}
static int append(int c, void *cl) {
	struct buf *p = cl;
	if (p->bp >= p->buf + p->size) {
		RESIZE(p->buf, 2*p->size);
		p->bp = p->buf + p->size;
		p->size *= 2;
	}
	*p->bp++ = c;
	return c;
}
void Fmt_puts(const char *str, int len,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision) {
	assert(str);
	assert(len >= 0);
	assert(flags);
	if (width == INT_MIN)
		width = 0;
	if (width < 0) {
		flags['-'] = 1;
		width = -width;
	}
	if (precision >= 0)
		flags['0'] = 0;
	if (precision >= 0 && precision < len)
		len = precision;
	if (!flags['-'])
		pad(width - len, ' ');
	{
		int i;
		for (i = 0; i < len; i++)
			put((unsigned char)*str++, cl);
	}
	if ( flags['-'])
		pad(width - len, ' ');
}
void Fmt_fmt(int put(int c, void *), void *cl,
	const char *fmt, ...) {
	va_list_box box;
	va_start(box.ap, fmt);
	Fmt_vfmt(put, cl, fmt, &box);
	va_end(box.ap);
}
void Fmt_print(const char *fmt, ...) {
	va_list_box box;
	va_start(box.ap, fmt);
	Fmt_vfmt(outc, stdout, fmt, &box);
	va_end(box.ap);
}
void Fmt_fprint(FILE *stream, const char *fmt, ...) {
	va_list_box box;
	va_start(box.ap, fmt);
	Fmt_vfmt(outc, stream, fmt, &box);
	va_end(box.ap);
}
int Fmt_sfmt(char *buf, int size, const char *fmt, ...) {
	int len;
	va_list_box box;
	va_start(box.ap, fmt);
	len = Fmt_vsfmt(buf, size, fmt, &box);
	va_end(box.ap);
	return len;
}
int Fmt_vsfmt(char *buf, int size, const char *fmt,
	va_list_box *box) {
	struct buf cl;
	assert(buf);
	assert(size > 0);
	assert(fmt);
	cl.buf = cl.bp = buf;
	cl.size = size;
	Fmt_vfmt(insert, &cl, fmt, box);
	insert(0, &cl);
	return cl.bp - cl.buf - 1;
}
char *Fmt_string(const char *fmt, ...) {
	char *str;
	va_list_box box;
	assert(fmt);	
	va_start(box.ap, fmt);
	str = Fmt_vstring(fmt, &box);
	va_end(box.ap);
	return str;
}
char *Fmt_vstring(const char *fmt, va_list_box *box) {
	struct buf cl;
	assert(fmt);
	cl.size = 256;
	cl.buf = cl.bp = ALLOC(cl.size);
	Fmt_vfmt(append, &cl, fmt, box);
	append(0, &cl);
	return RESIZE(cl.buf, cl.bp - cl.buf);
}
void Fmt_vfmt(int put(int c, void *cl), void *cl,
	const char *fmt, va_list_box *box) {
	assert(put);
	assert(fmt);
	while (*fmt)
		if (*fmt != '%' || *++fmt == '%')
			put((unsigned char)*fmt++, cl);
		else
			{
				unsigned char c, flags[256];
				int width = INT_MIN, precision = INT_MIN;
				memset(flags, '\0', sizeof flags);
				if (Fmt_flags) {
					unsigned char c = *fmt;
					for ( ; c && strchr(Fmt_flags, c); c = *++fmt) {
						assert(flags[c] < 255);
						flags[c]++;
					}
				}
				if (*fmt == '*' || isdigit(*fmt)) {
					int n;
					if (*fmt == '*') {
						n = va_arg(box->ap, int);
						assert(n != INT_MIN);
						fmt++;
					} else
						for (n = 0; isdigit(*fmt); fmt++) {
							int d = *fmt - '0';
							assert(n <= (INT_MAX - d)/10);
							n = 10*n + d;
						}
					width = n;
				}
				if (*fmt == '.' && (*++fmt == '*' || isdigit(*fmt))) {
					int n;
					if (*fmt == '*') {
						n = va_arg(box->ap, int);
						assert(n != INT_MIN);
						fmt++;
					} else
						for (n = 0; isdigit(*fmt); fmt++) {
							int d = *fmt - '0';
							assert(n <= (INT_MAX - d)/10);
							n = 10*n + d;
						}
					precision = n;
				}
				c = *fmt++;
				assert(cvt[c]);
				(*cvt[c])(c, box, put, cl, flags, width, precision);
			}
}
T Fmt_register(int code, T newcvt) {
	T old;
	assert(0 < code
		&& code < (int)(sizeof (cvt)/sizeof (cvt[0])));
	old = cvt[code];
	cvt[code] = newcvt;
	return old;
}
void Fmt_putd(const char *str, int len,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision) {
	int sign;
	assert(str);
	assert(len >= 0);
	assert(flags);
	if (width == INT_MIN)
		width = 0;
	if (width < 0) {
		flags['-'] = 1;
		width = -width;
	}
	if (precision >= 0)
		flags['0'] = 0;
	if (len > 0 && (*str == '-' || *str == '+')) {
		sign = *str++;
		len--;
	} else if (flags['+'])
		sign = '+';
	else if (flags[' '])
		sign = ' ';
	else
		sign = 0;
	{ int n;
	  if (precision < 0)
	  	precision = 1;
	  if (len < precision)
	  	n = precision;
	  else if (precision == 0 && len == 1 && str[0] == '0')
	  	n = 0;
	  else
	  	n = len;
	  if (sign)
	  	n++;
	  if (flags['-']) {
	  	if (sign)
			put(sign, cl);
	  } else if (flags['0']) {
	  	if (sign)
			put(sign, cl);
	  	pad(width - n, '0');
	  } else {
	  	pad(width - n, ' ');
	  	if (sign)
			put(sign, cl);
	  }
	  pad(precision - len, '0');
	  {
	  	int i;
	  	for (i = 0; i < len; i++)
	  		put((unsigned char)*str++, cl);
	  }
	  if (flags['-'])
	  	pad(width - n, ' '); }
}
