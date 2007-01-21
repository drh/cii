#include <string.h>
#include <limits.h>
#include "assert.h"
#include "fmt.h"
#include "text.h"
#include "mem.h"
#define T Text_T
#define idx(i,len) ((i) <= 0 ? (i) + (len) : (i) - 1)
#define swap(i,j) do { \
	int tt = (i); (i) = (j); (j) = tt; } while (0)
#define isatend(s,n) ((s).str + (s).len == current->avail \
	&& current->avail + (n) <= current->limit)
struct Text_save_T {
	struct chunk *current;
	unsigned char *avail;
};
static unsigned char cset[] = {
	  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
	 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
	 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
	 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
	112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
	128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
	144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
	160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
	176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
	192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
	208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
	224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
	240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255
};
const T Text_cset   = { 256, cset };
const T Text_ascii  = { 127, cset };
const T Text_ucase  = {  26, cset + 'A' };
const T Text_lcase  = {  26, cset + 'a' };
const T Text_digits = {  10, cset + '0' };
const T Text_null   = {   0, cset };
static struct chunk {
	struct chunk *link;
	unsigned char *avail;
	unsigned char *limit;
} head, *current = &head;
static unsigned char *alloc(int len) {
	assert(len >= 0);
	if (current->avail + len > current->limit) {
		current = current->link = ALLOC(sizeof *current
							+ 10*1024 + len);
		current->avail = (void *)(current + 1);
		current->limit = current->avail + 10*1024 + len;
	}
	current->avail += len;
	return current->avail - len;
}
static int inset(unsigned char *set, int len, int c) {
	int i;
	for (i = 0; i < len; i++)
		if (c == set[i])
			return 1;
	return 0;
}
static int equal(unsigned char *s1, unsigned char *s2,
	int len) {
	for ( ; len-- > 0; s1++, s2++)
		if (*s1 != *s2)
			return 0;
	return 1;
}
int Text_pos(T s, int i) {
	assert(s.len >= 0 && s.str);
	i = idx(i, s.len);
	assert(i >= 0 && i <= s.len);
	return i + 1;
}
T Text_box(char *str, int len) {
	T text;
	assert(str);
	assert(len >= 0);
	text.str = (unsigned char *)str;
	text.len = len;
	return text;
}
T Text_sub(T s, int i, int j) {
	T text;
	assert(s.len >= 0 && s.str);
	i = idx(i, s.len);
	j = idx(j, s.len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= s.len);
	assert(j >= 0 && j <= s.len);
	text.len = j - i;
	text.str = s.str + i;
	return text;
}
T Text_put(const char *str) {
	T text;
	assert(str);
	text.len = strlen(str);
	text.str = alloc(text.len);
	memcpy(text.str, str, text.len);
	return text;
}
char *Text_get(char *str, int size, T s) {
	assert(s.len >= 0 && s.str);
	if (str == NULL) {
		size = s.len + 1;
		str = ALLOC(size);
	}
	assert(size >= s.len + 1);
	memcpy(str, s.str, s.len);
	str[s.len] = '\0';
	return str;
}
T Text_dup(T s, int n) {
	assert(s.len >= 0 && s.str);
	assert(n >= 0);
	if (n == 0 || s.len == 0)
		return Text_null;
	if (n == 1)
		return s;
	{
		T text;
		unsigned char *p;
		text.len = n*s.len;
		if (isatend(s, text.len - s.len)) {
			text.str = s.str;
			p = alloc(text.len - s.len);
			n--;
		} else
			p = text.str = alloc(text.len);
		for ( ; n-- > 0; p += s.len)
			memcpy(p, s.str, s.len);
		return text;
	}
}
T Text_cat(T s1, T s2) {
	assert(s1.len >= 0 && s1.str);
	assert(s2.len >= 0 && s2.str);
	if (s1.len == 0)
		return s2;
	if (s2.len == 0)
		return s1;
	if (s1.str + s1.len == s2.str) {
		s1.len += s2.len;
		return s1;
	}
	{
		T text;
		text.len = s1.len + s2.len;
		if (isatend(s1, s2.len)) {
			text.str = s1.str;
			(void)alloc(s2.len);
		} else {
			text.str = alloc(s1.len + s2.len);
			memcpy(text.str, s1.str, s1.len);
		}
		memcpy(text.str + s1.len, s2.str, s2.len);
		return text;
	} 
}
T Text_reverse(T s) {
	assert(s.len >= 0 && s.str);
	if (s.len == 0)
		return Text_null;
	else if (s.len == 1)
		return s;
	else {
		T text;
		unsigned char *p;
		int i = s.len;
		text.len = s.len;
		p = text.str = alloc(s.len);
		while (--i >= 0)
			*p++ = s.str[i];
		return text;
	}
}
T Text_map(T s, const T *from, const T *to) {
	static unsigned char map[256];
	static int inited = 0;
	assert(s.len >= 0 && s.str);
	if (from && to) {
		int k;
		for (k = 0; k < (int)sizeof map; k++)
			map[k] = k;
		assert(from->len == to->len);
		for (k = 0; k < from->len; k++)
			map[from->str[k]] = to->str[k];
		inited = 1;
	} else {
		assert(inited);
		assert(from == NULL && to == NULL);
	}
	if (s.len == 0)
		return Text_null;
	else {
		T text;
		int i;
		text.len = s.len;
		text.str = alloc(s.len);
		for (i = 0; i < s.len; i++)
			text.str[i] = map[s.str[i]];
		return text;
	}
}
Text_save_T Text_save(void) {
	Text_save_T save;
	NEW(save);
	save->current = current;
	save->avail = current->avail;
	(void)alloc(1);
	return save;
}
void Text_restore(Text_save_T *save) {
	struct chunk *p, *q;
	assert(save && *save);
	current = (*save)->current;
	current->avail = (*save)->avail;
	FREE(save);
	for (p = current->link; p; p = q) {
		q = p->link;
		FREE(p);
	}
	current->link = NULL;
}
int Text_cmp(T s1, T s2) {
	int i;
	assert(s1.len >= 0 && s1.str);
	assert(s2.len >= 0 && s2.str);
	if (s1.str == s2.str)
		return s1.len - s2.len;
	for (i = 0; i < s1.len && i < s2.len; i++)
		if (s1.str[i] != s2.str[i])
			return s1.str[i] - s2.str[i];
	if (s1.len < s2.len)
		return -1;
	else if (s1.len > s2.len)
		return +1;
	else
		return 0;
}
int Text_chr(T s, int i, int j, int c) {
	assert(s.len >= 0 && s.str);
	i = idx(i, s.len);
	j = idx(j, s.len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= s.len);
	assert(j >= 0 && j <= s.len);
	for ( ; i < j; i++)
		if (s.str[i] == c)
			return i + 1;
	return 0;
}
int Text_rchr(T s, int i, int j, int c) {
	assert(s.len >= 0 && s.str);
	i = idx(i, s.len);
	j = idx(j, s.len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= s.len);
	assert(j >= 0 && j <= s.len);
	while (j > i)
		if (s.str[--j] == c)
			return j + 1;
	return 0;
}
int Text_upto(T s, int i, int j, T set) {
	assert(set.len >= 0 && set.str);
	assert(s.len >= 0 && s.str);
	i = idx(i, s.len);
	j = idx(j, s.len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= s.len);
	assert(j >= 0 && j <= s.len);
	for ( ; i < j; i++)
		if (inset(set.str, set.len, s.str[i]))
			return i + 1;
	return 0;
}
int Text_rupto(T s, int i, int j, T set) {
	assert(set.len >= 0 && set.str);
	assert(s.len >= 0 && s.str);
	i = idx(i, s.len);
	j = idx(j, s.len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= s.len);
	assert(j >= 0 && j <= s.len);
	while (j > i)
		if (inset(set.str, set.len, s.str[--j]))
			return j + 1;
	return 0;
}
int Text_find(T s, int i, int j, T pat) {
	assert(pat.len >= 0 && pat.str);
	assert(s.len >= 0 && s.str);
	i = idx(i, s.len);
	j = idx(j, s.len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= s.len);
	assert(j >= 0 && j <= s.len);
	if (pat.len == 0)
		return i + 1;
	else if (pat.len == 1) {
		for ( ; i < j; i++)
			if (s.str[i] == *pat.str)
				return i + 1;
	} else
		for ( ; i + pat.len <= j; i++)
			if (equal(&s.str[i], pat.str, pat.len))
				return i + 1;
	return 0;
}
int Text_rfind(T s, int i, int j, T pat) {
	assert(pat.len >= 0 && pat.str);
	assert(s.len >= 0 && s.str);
	i = idx(i, s.len);
	j = idx(j, s.len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= s.len);
	assert(j >= 0 && j <= s.len);
	if (pat.len == 0)
		return j;
	else if (pat.len == 1) {
		while (j > i)
			if (s.str[--j] == *pat.str)
				return j + 1;
	} else
		for ( ; j - pat.len >= i; j--)
			if (equal(&s.str[j-pat.len], pat.str, pat.len))
				return j - pat.len + 1;
	return 0;
}
int Text_any(T s, int i, T set) {
	assert(s.len >= 0 && s.str);
	assert(set.len >= 0 && set.str);
	i = idx(i, s.len);
	assert(i >= 0 && i <= s.len);
	if (i < s.len && inset(set.str, set.len, s.str[i]))
		return i + 2;
	return 0;
}
int Text_many(T s, int i, int j, T set) {
	assert(set.len >= 0 && set.str);
	assert(s.len >= 0 && s.str);
	i = idx(i, s.len);
	j = idx(j, s.len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= s.len);
	assert(j >= 0 && j <= s.len);
	if (i < j && inset(set.str, set.len, s.str[i])) {
		do
			i++;
		while (i < j && inset(set.str, set.len, s.str[i]));
		return i + 1;
	}
	return 0;
}
int Text_rmany(T s, int i, int j, T set) {
	assert(set.len >= 0 && set.str);
	assert(s.len >= 0 && s.str);
	i = idx(i, s.len);
	j = idx(j, s.len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= s.len);
	assert(j >= 0 && j <= s.len);
	if (j >= i && inset(set.str, set.len, s.str[j-1])) {
		do
			--j;
		while (j >= i && inset(set.str, set.len, s.str[j]));
		return j + 1;
	}
	return 0;
}
int Text_match(T s, int i, int j, T pat) {
	assert(pat.len >= 0 && pat.str);
	assert(s.len >= 0 && s.str);
	i = idx(i, s.len);
	j = idx(j, s.len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= s.len);
	assert(j >= 0 && j <= s.len);
	if (pat.len == 0)
		return i + 1;
	else if (pat.len == 1) {
		if (i < j && s.str[i] == *pat.str)
			return i + 2;
	} else if (i + pat.len <= j
	&& equal(&s.str[i], pat.str, pat.len))
		return i + pat.len + 1;
	return 0;
}
int Text_rmatch(T s, int i, int j, T pat) {
	assert(pat.len >= 0 && pat.str);
	assert(s.len >= 0 && s.str);
	i = idx(i, s.len);
	j = idx(j, s.len);
	if (i > j)
		swap(i,j);
	assert(i >= 0 && i <= s.len);
	assert(j >= 0 && j <= s.len);
	if (pat.len == 0)
		return j + 1;
	else if (pat.len == 1) {
		if (j > i && s.str[j-1] == *pat.str)
			return j;
	} else if (j - pat.len >= i
	&& equal(&s.str[j-pat.len], pat.str, pat.len))
		return j - pat.len + 1;
	return 0;
}
void Text_cvt(int code, va_list *app,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision) {
	T *s = va_arg(*app, T*);
	Fmt_puts((char *)s->str, s->len, put, cl, flags,
		width, precision);
}
static char rcsid[] = "$RCSfile: RCS/text.doc,v $ $Revision: 1.2 $";
