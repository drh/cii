static char rcsid[] = "$Id$";
#include <string.h>
#include <limits.h>
#include "assert.h"
#include "fmt.h"
#include "str.h"
#include "mem.h"
#define idx(i, len) ((i) <= 0 ? (i) + (len) : (i) - 1)
#define convert(s, i, j) do { int len; \
	assert(s); len = strlen(s); \
	i = idx(i, len); j = idx(j, len); \
	if (i > j) { int t = i; i = j; j = t; } \
	assert(i >= 0 && j <= len); } while (0)
char *Str_sub(const char *s, int i, int j) {
	char *str, *p;
	convert(s, i, j);
	p = str = ALLOC(j - i + 1);
	while (i < j)
		*p++ = s[i++];
	*p = '\0';
	return str;
}
char *Str_dup(const char *s, int i, int j, int n) {
	int k;
	char *str, *p;
	assert(n >= 0);
	convert(s, i, j);
	p = str = ALLOC(n*(j - i) + 1);
	if (j - i > 0)
		while (n-- > 0)
			for (k = i; k < j; k++)
				*p++ = s[k];
	*p = '\0';
	return str;
}
char *Str_reverse(const char *s, int i, int j) {
	char *str, *p;
	convert(s, i, j);
	p = str = ALLOC(j - i + 1);
	while (j > i)
		*p++ = s[--j];
	*p = '\0';
	return str;
}
char *Str_cat(const char *s1, int i1, int j1,
              const char *s2, int i2, int j2) {
	char *str, *p;
	convert(s1, i1, j1);
	convert(s2, i2, j2);
	p = str = ALLOC(j1 - i1 + j2 - i2 + 1);
	while (i1 < j1)
		*p++ = s1[i1++];
	while (i2 < j2)
		*p++ = s2[i2++];
	*p = '\0';
	return str;
}
char *Str_catv(const char *s, ...) {
	char *str, *p;
	const char *save = s;
	int i, j, len = 0;
	va_list ap;
	va_start(ap, s);
	while (s) {
		i = va_arg(ap, int);
		j = va_arg(ap, int);
		convert(s, i, j);
		len += j - i;
		s = va_arg(ap, const char *);
	}
	va_end(ap);
	p = str = ALLOC(len + 1);
	s = save;
	va_start(ap, s);
	while (s) {
		i = va_arg(ap, int);
		j = va_arg(ap, int);
		convert(s, i, j);
		while (i < j)
			*p++ = s[i++];
		s = va_arg(ap, const char *);
	}
	va_end(ap);
	*p = '\0';
	return str;
}
char *Str_map(const char *s, int i, int j,
	const char *from, const char *to) {
	static char map[256] = { 0 };
	if (from && to) {
		unsigned c;
		for (c = 0; c < sizeof map; c++)
			map[c] = c;
		while (*from && *to)
			map[(unsigned char)*from++] = *to++;
		assert(*from == 0 && *to == 0);
	} else {
		assert(from == NULL && to == NULL && s);
		assert(map['a']);
	}
	if (s) {
		char *str, *p;
		convert(s, i, j);
		p = str = ALLOC(j - i + 1);
		while (i < j)
			*p++ = map[(unsigned char)s[i++]];
		*p = '\0';
		return str;
	} else
		return NULL;
}
int Str_pos(const char *s, int i) {
	int len;
	assert(s);
	len = strlen(s);
	i = idx(i, len);
	assert(i >= 0 && i <= len);
	return i + 1;
}
int Str_len(const char *s, int i, int j) {
	convert(s, i, j);
	return j - i;
}
int Str_cmp(const char *s1, int i1, int j1,
	const char *s2, int i2, int j2) {
	convert(s1, i1, j1);
	convert(s2, i2, j2);
	s1 += i1;
	s2 += i2;
	if (j1 - i1 < j2 - i2) {
		int cond = strncmp(s1, s2, j1 - i1);
		return cond == 0 ? -1 : cond;
	} else if (j1 - i1 > j2 - i2) {
		int cond = strncmp(s1, s2, j2 - i2);
		return cond == 0 ? +1 : cond;
	} else
		return strncmp(s1, s2, j1 - i1);
}
int Str_chr(const char *s, int i, int j, int c) {
	convert(s, i, j);
	for ( ; i < j; i++)
		if (s[i] == c)
			return i + 1;
	return 0;
}
int Str_rchr(const char *s, int i, int j, int c) {
	convert(s, i, j);
	while (j > i)
		if (s[--j] == c)
			return j + 1;
	return 0;
}
int Str_upto(const char *s, int i, int j,
	const char *set) {
	assert(set);
	convert(s, i, j);
	for ( ; i < j; i++)
		if (strchr(set, s[i]))
			return i + 1;
	return 0;
}
int Str_rupto(const char *s, int i, int j,
	const char *set) {
	assert(set);
	convert(s, i, j);
	while (j > i)
		if (strchr(set, s[--j]))
			return j + 1;
	return 0;
}
int Str_find(const char *s, int i, int j,
	const char *str) {
	int len;
	convert(s, i, j);
	assert(str);
	len = strlen(str);
	if (len == 0)
		return i + 1;
	else if (len == 1) {
		for ( ; i < j; i++)
			if (s[i] == *str)
				return i + 1;
	} else
		for ( ; i + len <= j; i++)
			if ((strncmp(&s[i], str, len) == 0))
				return i + 1;
	return 0;
}
int Str_rfind(const char *s, int i, int j,
	const char *str) {
	int len;
	convert(s, i, j);
	assert(str);
	len = strlen(str);
	if (len == 0)
		return j + 1;
	else if (len == 1) {
		while (j > i)
			if (s[--j] == *str)
				return j + 1;
	} else
		for ( ; j - len >= i; j--)
			if (strncmp(&s[j-len], str, len) == 0)
				return j - len + 1;
	return 0;
}
int Str_any(const char *s, int i, const char *set) {
	int len;
	assert(s);
	assert(set);
	len = strlen(s);
	i = idx(i, len);
	assert(i >= 0 && i <= len);
	if (i < len && strchr(set, s[i]))
		return i + 2;
	return 0;
}
int Str_many(const char *s, int i, int j,
	const char *set) {
	assert(set);
	convert(s, i, j);
	if (i < j && strchr(set, s[i])) {
		do
			i++;
		while (i < j && strchr(set, s[i]));
		return i + 1;
	}
	return 0;
}
int Str_rmany(const char *s, int i, int j,
	const char *set) {
	assert(set);
	convert(s, i, j);
	if (j > i && strchr(set, s[j-1])) {
		do
			--j;
		while (j >= i && strchr(set, s[j]));
		return j + 2;
	}
	return 0;
}
int Str_match(const char *s, int i, int j,
	const char *str) {
	int len;
	convert(s, i, j);
	assert(str);
	len = strlen(str);
	if (len == 0)
		return i + 1; 
	else if (len == 1) {
		if (i < j && s[i] == *str)
			return i + 2;
	} else if (i + len <= j && (strncmp(&s[i], str, len) == 0))
		return i + len + 1;
	return 0;
}
int Str_rmatch(const char *s, int i, int j,
	const char *str) {
	int len;
	convert(s, i, j);
	assert(str);
	len = strlen(str);
	if (len == 0)
		return j + 1;
	else if (len == 1) {
		if (j > i && s[j-1] == *str)
			return j;
	} else if (j - len >= i
	&& strncmp(&s[j-len], str, len) == 0)
		return j - len + 1;
	return 0;
}
void Str_fmt(int code, va_list_box *box,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision) {
	char *s;
	int i, j;
	assert(box && flags);
	s = va_arg(box->ap, char *);
	i = va_arg(box->ap, int);
	j = va_arg(box->ap, int);
	convert(s, i, j);
	Fmt_puts(s + i, j - i, put, cl, flags,
		width, precision);
}
