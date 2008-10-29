/* $Id$ */
#ifndef TEXT_INCLUDED
#define TEXT_INCLUDED
#include <stdarg.h>
#include "fmt.h"
#define T Text_T
typedef struct T {
	int len;
	const char *str;
} T;
typedef struct Text_save_T *Text_save_T;
extern const T Text_cset;
extern const T Text_ascii;
extern const T Text_ucase;
extern const T Text_lcase;
extern const T Text_digits;
extern const T Text_null;
extern T     Text_put(const char *str);
extern char *Text_get(char *str, int size, T s);
extern T     Text_box(const char *str, int len);
extern T Text_sub(T s, int i, int j);
extern int Text_pos(T s, int i);
extern T Text_cat    (T s1, T s2);
extern T Text_dup    (T s, int n);
extern T Text_reverse(T s);
extern T Text_map(T s, const T *from, const T *to);
extern int Text_cmp(T s1, T s2);
extern int Text_chr  (T s, int i, int j, int c);
extern int Text_rchr (T s, int i, int j, int c);
extern int Text_upto (T s, int i, int j, T set);
extern int Text_rupto(T s, int i, int j, T set);
extern int Text_any  (T s, int i, T set);
extern int Text_many (T s, int i, int j, T set);
extern int Text_rmany(T s, int i, int j, T set);
extern int Text_find  (T s, int i, int j, T str);
extern int Text_rfind (T s, int i, int j, T str);
extern int Text_match (T s, int i, int j, T str);
extern int Text_rmatch(T s, int i, int j, T str);
extern void Text_fmt(int code, va_list_box *box,
	int put(int c, void *cl), void *cl,
	unsigned char flags[], int width, int precision);
extern Text_save_T Text_save(void);
extern void        Text_restore(Text_save_T *save);
#undef T
#endif
