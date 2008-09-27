/* $Id$ */
#ifndef STR_INCLUDED
#define STR_INCLUDED
#include <stdarg.h>
#include "fmt.h"
extern char *Str_sub(const char *s, int i, int j);
extern char *Str_dup(const char *s, int i, int j, int n);
extern char *Str_cat(const char *s1, int i1, int j1,
	const char *s2, int i2, int j2);
extern char *Str_catv   (const char *s, ...);
extern char *Str_reverse(const char *s, int i, int j);
extern char *Str_map    (const char *s, int i, int j,
	const char *from, const char *to);
extern int Str_pos(const char *s, int i);
extern int Str_len(const char *s, int i, int j);
extern int Str_cmp(const char *s1, int i1, int j1,
	const char *s2, int i2, int j2);
extern int Str_chr  (const char *s, int i, int j, int c);
extern int Str_rchr (const char *s, int i, int j, int c);
extern int Str_upto (const char *s, int i, int j,
	const char *set);
extern int Str_rupto(const char *s, int i, int j,
	const char *set);
extern int Str_find (const char *s, int i, int j,
	const char *str);
extern int Str_rfind(const char *s, int i, int j,
	const char *str);
extern int Str_any   (const char *s, int i,
	const char *set);
extern int Str_many  (const char *s, int i, int j,
	const char *set);
extern int Str_rmany (const char *s, int i, int j,
	const char *set);
extern int Str_match (const char *s, int i, int j,
	const char *str);
extern int Str_rmatch(const char *s, int i, int j,
	const char *str);
extern void Str_fmt(int code, va_list_box *box,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision);
#undef T
#endif
