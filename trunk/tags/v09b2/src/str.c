#include <string.h>
#include <limits.h>
#include "assert.h"
#include "fmt.h"
#include "str.h"
#include "mem.h"
#define idx(i,len) ((i) <= 0 ? (i) + (len) : (i) - 1)
	#define swap(i,j) do { \
		int tt = (i); (i) = (j); (j) = tt; } while (0)
char *Str_sub(const char *s, int i, int j) {
	int len;
	char *str, *p;
	assert(s);
	len = strlen(s);
	i = idx(i, len);
	j = idx(j, len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= len);
	assert(j >= 0 && j <= len);
	p = str = ALLOC(j - i + 1);
	while (i < j)
		*p++ = s[i++];
	*p = 0;
	return str;
}
char *Str_dup(const char *s, int i, int j, int n) {
	int len, k;
	char *str, *p;
	assert(s);
	assert(n >= 0);
	len = strlen(s);
	i = idx(i, len);
	j = idx(j, len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= len);
	assert(j >= 0 && j <= len);
	p = str = ALLOC(n*(j - i) + 1);
	while (n-- > 0)
		for (k = i; k < j; k++)
			*p++ = s[k];
	*p = 0;
	return str;
}
char *Str_reverse(const char *s, int i, int j) {
	int len;
	char *str, *p;
	assert(s);
	len = strlen(s);
	i = idx(i, len);
	j = idx(j, len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= len);
	assert(j >= 0 && j <= len);
	p = str = ALLOC(j - i + 1);
	while (j > i)
		*p++ = s[--j];
	*p = 0;
	return str;
}
char *Str_cat(const char *s1, int i1, int j1,
	const char *s2, int i2, int j2) {
	assert(s1);
	assert(s2);
	return Str_catv(s1, i1, i2, s2, i2, j2, NULL);
}
char *Str_catv(const char *s, ...) {
	char *str, *p;
	int i, j, len, newlen = 0;
	va_list ap;
	va_start(ap, s);
	while (s) {
		i = va_arg(ap, int);
		j = va_arg(ap, int);
		len = strlen(s);
		i = idx(i, len);
		j = idx(j, len);
		if (i > j)
			swap(i,j);
		assert(i >= 0 && i <= len);
		assert(j >= 0 && j <= len);
		newlen += j - i;
		s = va_arg(ap, char *);
	}
	va_end(ap);
	p = str = ALLOC(newlen + 1);
	va_start(ap, s);
	while (s) {
		i = va_arg(ap, int);
		j = va_arg(ap, int);
		len = strlen(s);
		i = idx(i, len);
		j = idx(j, len);
		if (i > j)
			swap(i,j);
		assert(i >= 0 && i <= len);
		assert(j >= 0 && j <= len);
		while (i < j)
			*p++ = s[i++];
		s = va_arg(ap, char *);
	}
	va_end(ap);
	*p = 0;
	return str;
}
char *Str_map(const char *s, int i, int j,
	const char *from, const char *to) {
	static char map[256] = { 0 };
	if (from && to) {
		int k;
		for (k = 0; k < (int)sizeof map; k++)
			map[k] = k;
		while (*from && *to)
			map[(unsigned char)*from++] = *to++;
		assert(*from == 0 && *to == 0);
	} else {
		assert(from == NULL && to == NULL && s);
		assert(map['a']);
	}
	if (s) {
		char *str, *p;
		int len = strlen(s);
i = idx(i, len);
j = idx(j, len);
if (i > j)
	swap(i,j);
assert(i >= 0 && i <= len);
assert(j >= 0 && j <= len);
		p = str = ALLOC(j - i + 1);
		while (i < j)
			*p++ = map[(unsigned char)s[i++]];
		*p = 0;
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
	int len;
	assert(s);
	len = strlen(s);
	i = idx(i, len);
	j = idx(j, len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= len);
	assert(j >= 0 && j <= len);
	return j - i;
}
int Str_cmp(const char *s1, int i1, int j1,
	const char *s2, int i2, int j2) {
	int len;
	assert(s1);
	len = strlen(s1);
	i1 = idx(i1, len);
	j1 = idx(j1, len);
	if (i1 > j1)
		swap(i1, j1);
	assert(i1 >= 0 && i1 <= len && j1 >= 0 && j1 <= len);
	assert(s2);
	len = strlen(s2);
	i2 = idx(i2, len);
	j2 = idx(j2, len);
	if (i2 > j2)
		swap(i2, j2);
	assert(i2 >= 0 && i2 <= len && j2 >= 0 && j2 <= len);
	if (j1 - i1 < j2 - i2)
		len = j1 - i1;
	else
		len = j2 - i2;
	s1 += i1;
	s2 += i2;
	for ( ; len-- > 0; s1++, s2++)
		if (*s1 != *s2)
			return (unsigned char)*s1 - (unsigned char)*s2;
	if (j1 - i1 < j2 - i2)
		return -1;
	else if (j1 - i1 < j2 - i2)
		return +1;
	else
		return 0;
}
int Str_chr(const char *s, int i, int j, int c) {
	int len;
	assert(s);
	len = strlen(s);
	i = idx(i, len);
	j = idx(j, len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= len);
	assert(j >= 0 && j <= len);
	for ( ; i < j; i++)
		if (s[i] == c)
			return i + 1;
	return 0;
}
int Str_rchr(const char *s, int i, int j, int c) {
	int len;
	assert(s);
	len = strlen(s);
	i = idx(i, len);
	j = idx(j, len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= len);
	assert(j >= 0 && j <= len);
	while (j > i)
		if (s[--j] == c)
			return j + 1;
	return 0;
}
int Str_upto(const char *s, int i, int j, const char *set) {
	int len;
	assert(s);
	assert(set);
	len = strlen(s);
	i = idx(i, len);
	j = idx(j, len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= len);
	assert(j >= 0 && j <= len);
	for ( ; i < j; i++)
		if (strchr(set, s[i]))
			return i + 1;
	return 0;
}
int Str_rupto(const char *s, int i, int j, const char *set) {
	int len;
	assert(s);
	assert(set);
	len = strlen(s);
	i = idx(i, len);
	j = idx(j, len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= len);
	assert(j >= 0 && j <= len);
	while (j > i)
		if (strchr(set, s[--j]))
			return j + 1;
	return 0;
}
int Str_find(const char *s, int i, int j, const char *pat) {
	int len, plen;
	assert(s);
	len = strlen(s);
	i = idx(i, len);
	j = idx(j, len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= len);
	assert(j >= 0 && j <= len);
	assert(pat);
	plen = strlen(pat);
	if (plen == 0)
		return i + 1;
	else if (plen == 1) {
		for ( ; i < j; i++)
			if (s[i] == *pat)
				return i + 1;
	} else
		for ( ; i + plen <= j; i++)
			if ((strncmp(&s[i], pat, plen) == 0))
				return i + 1;
	return 0;
}
int Str_rfind(const char *s, int i, int j, const char *pat) {
	int len, plen;
	assert(s);
	len = strlen(s);
	i = idx(i, len);
	j = idx(j, len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= len);
	assert(j >= 0 && j <= len);
	assert(pat);
	plen = strlen(pat);
	if (plen == 0)
		return j;
	else if (plen == 1) {
		while (j > i)
			if (s[--j] == *pat)
				return j + 1;
	} else
		for ( ; j - plen >= i; j--)
			if (strncmp(&s[j-plen], pat, plen) == 0)
				return j - plen + 1;
	return 0;
}
int Str_any(const char *s, int i, const char *set) {
	int len;
	assert(s);
	assert(set);
	len = strlen(s);
	i = idx(i, len);
	assert(i >= 0 && i <= len);
	if (strchr(set, s[i]))
		return i + 2;
	return 0;
}
int Str_many(const char *s, int i, int j, const char *set) {
	int len;
	assert(s);
	assert(set);
	len = strlen(s);
	i = idx(i, len);
	j = idx(j, len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= len);
	assert(j >= 0 && j <= len);
	if (strchr(set, s[i])) {
		do
			i++;
		while (i < j && strchr(set, s[i]));
		return i + 1;
	}
	return 0;
}
int Str_rmany(const char *s, int i, int j, const char *set) {
	int len;
	assert(s);
	assert(set);
	len = strlen(s);
	i = idx(i, len);
	j = idx(j, len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= len);
	assert(j >= 0 && j <= len);
	if (strchr(set, s[j-1])) {
		do
			--j;
		while (j >= i && strchr(set, s[j]));
		return j + 1;
	}
	return 0;
}
int Str_match(const char *s, int i, int j, const char *pat) {
	int len, plen;
	assert(s);
	len = strlen(s);
	i = idx(i, len);
	j = idx(j, len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= len);
	assert(j >= 0 && j <= len);
	assert(pat);
	plen = strlen(pat);
	if (plen == 0)
		return i + 1; 
	else if (plen == 1) {
		if (s[i] == *pat)
			return i + 2;
	} else if (i + plen <= j && (strncmp(&s[i], pat, plen) == 0))
		return i + plen + 1;
	return 0;
}
int Str_rmatch(const char *s, int i, int j, const char *pat) {
	int len, plen;
	assert(s);
	len = strlen(s);
	i = idx(i, len);
	j = idx(j, len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= len);
	assert(j >= 0 && j <= len);
	assert(pat);
	plen = strlen(pat);
	if (plen == 0)
		return j + 1;
	else if (plen == 1) {
		if (s[j-1] == *pat)
			return j;
	} else if (j - plen >= i
	&& strncmp(&s[j-plen], pat, plen) == 0)
		return j - plen + 1;
	return 0;
}
void Str_cvt(int code, va_list *app,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision) {
	char *s = va_arg(*app, char *);
	int i = va_arg(*app, int), j = va_arg(*app, int), len;
	assert(s);
	len = strlen(s);
	i = idx(i, len);
	j = idx(j, len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= len);
	assert(j >= 0 && j <= len);
	Fmt_puts(s + i, j - i, put, cl, flags, width, precision);
}
static char rcsid[] = "$RCSfile: RCS/str.doc,v $ $Revision: 1.2 $";
