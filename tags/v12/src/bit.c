static char rcsid[] = "$Id$";
#include <stdarg.h>
#include <string.h>
#include "assert.h"
#include "bit.h"
#include "mem.h"
#define T Bit_T
struct T {
	int length;
	unsigned char *bytes;
	unsigned long *words;
};
#define BPW (8*sizeof (unsigned long))
#define nwords(len) ((((len) + BPW - 1)&(~(BPW-1)))/BPW)
#define nbytes(len) ((((len) + 8 - 1)&(~(8-1)))/8)
#define setop(sequal, snull, tnull, op) \
	if (s == t) { assert(s); return sequal; } \
	else if (s == NULL) { assert(t); return snull; } \
	else if (t == NULL) return tnull; \
	else { \
		int i; T set; \
		assert(s->length == t->length); \
		set = Bit_new(s->length); \
		for (i = nwords(s->length); --i >= 0; ) \
			set->words[i] = s->words[i] op t->words[i]; \
		return set; }
unsigned char msbmask[] = {
	0xFF, 0xFE, 0xFC, 0xF8,
	0xF0, 0xE0, 0xC0, 0x80
};
unsigned char lsbmask[] = {
	0x01, 0x03, 0x07, 0x0F,
	0x1F, 0x3F, 0x7F, 0xFF
};
static T copy(T t) {
	T set;
	assert(t);
	set = Bit_new(t->length);
	if (t->length > 0)
		memcpy(set->bytes, t->bytes, nbytes(t->length));
	return set;
}
T Bit_new(int length) {
	T set;
	assert(length >= 0);
	NEW(set);
	if (length > 0)
		set->words = CALLOC(nwords(length),
			sizeof (unsigned long));
	else
		set->words = NULL;
	set->bytes = (unsigned char *)set->words;
	set->length = length;
	return set;
}
void Bit_free(T *set) {
	assert(set && *set);
	FREE((*set)->words);
	FREE(*set);
}
int Bit_length(T set) {
	assert(set);
	return set->length;
}
int Bit_count(T set) {
	int length = 0, n;
	static char count[] = {
		0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4 };
	assert(set);
	for (n = nbytes(set->length); --n >= 0; ) {
		unsigned char c = set->bytes[n];
		length += count[c&0xF] + count[c>>4];
	}
	return length;
}
int Bit_get(T set, int n) {
	assert(set);
	assert(0 <= n && n < set->length);
	return ((set->bytes[n/8]>>(n%8))&1);
}
int Bit_put(T set, int n, int bit) {
	int prev;
	assert(set);
	assert(bit == 0 || bit == 1);
	assert(0 <= n && n < set->length);
	prev = ((set->bytes[n/8]>>(n%8))&1);
	if (bit == 1)
		set->bytes[n/8] |=   1<<(n%8);
	else
		set->bytes[n/8] &= ~(1<<(n%8));
	return prev;
}
void Bit_set(T set, int lo, int hi) {
		assert(set);
		assert(0 <= lo && hi < set->length);
		assert(lo <= hi);
	if (lo/8 < hi/8) {
		set->bytes[lo/8] |= msbmask[lo%8];
		{
			int i;
			for (i = lo/8+1; i < hi/8; i++)
				set->bytes[i] = 0xFF;
		}
		set->bytes[hi/8] |= lsbmask[hi%8];
	} else
		set->bytes[lo/8] |= (msbmask[lo%8]&lsbmask[hi%8]);
}
void Bit_clear(T set, int lo, int hi) {
		assert(set);
		assert(0 <= lo && hi < set->length);
		assert(lo <= hi);
	if (lo/8 < hi/8) {
		int i;
		set->bytes[lo/8] &= ~msbmask[lo%8];
		for (i = lo/8+1; i < hi/8; i++)
			set->bytes[i] = 0;
		set->bytes[hi/8] &= ~lsbmask[hi%8];
	} else
		set->bytes[lo/8] &= ~(msbmask[lo%8]&lsbmask[hi%8]);
}
void Bit_not(T set, int lo, int hi) {
		assert(set);
		assert(0 <= lo && hi < set->length);
		assert(lo <= hi);
	if (lo/8 < hi/8) {
		int i;
		set->bytes[lo/8] ^= msbmask[lo%8];
		for (i = lo/8+1; i < hi/8; i++)
			set->bytes[i] ^= 0xFF;
		set->bytes[hi/8] ^= lsbmask[hi%8];
	} else
		set->bytes[lo/8] ^= (msbmask[lo%8]&lsbmask[hi%8]);
}
void Bit_map(T set,
	void apply(int n, int bit, void *cl), void *cl) {
	int n;
	assert(set);
	for (n = 0; n < set->length; n++)
		apply(n, ((set->bytes[n/8]>>(n%8))&1), cl);
}
int Bit_eq(T s, T t) {
	int i;
	assert(s && t);
	assert(s->length == t->length);
	for (i = nwords(s->length); --i >= 0; )
		if (s->words[i] != t->words[i])
			return 0;
	return 1;
}
int Bit_leq(T s, T t) {
	int i;
	assert(s && t);
	assert(s->length == t->length);
	for (i = nwords(s->length); --i >= 0; )
		if ((s->words[i]&~t->words[i]) != 0)
			return 0;
	return 1;
}
int Bit_lt(T s, T t) {
	int i, lt = 0;
	assert(s && t);
	assert(s->length == t->length);
	for (i = nwords(s->length); --i >= 0; )
		if ((s->words[i]&~t->words[i]) != 0)
			return 0;
		else if (s->words[i] != t->words[i])
			lt |= 1;
	return lt;
}
T Bit_union(T s, T t) {
	setop(copy(t), copy(t), copy(s), |)
}
T Bit_inter(T s, T t) {
	setop(copy(t),
		Bit_new(t->length), Bit_new(s->length), &)
}
T Bit_minus(T s, T t) {
	setop(Bit_new(s->length),
		Bit_new(t->length), copy(s), & ~)
}
T Bit_diff(T s, T t) {
	setop(Bit_new(s->length), copy(t), copy(s), ^)
}
