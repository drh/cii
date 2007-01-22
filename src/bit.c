#include <stdarg.h>
#include <string.h>
#include "assert.h"
#include "bit.h"
#include "mem.h"
#define T Bit_T
struct T {
	int size;
	union {
		unsigned char *bytes;
		unsigned int  *words;
	} u;
};
#define BPW (8*sizeof (unsigned))
#define nwords(size) ((((size) + BPW - 1)&(~(BPW-1)))/BPW)
#define nbytes(size) ((((size) +   8 - 1)&(~(  8-1)))/  8)
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
	set = Bit_new(t->size);
	if (t->size > 0)
		memcpy(set->u.bytes, t->u.bytes, nbytes(t->size));
	return set;
}
T Bit_new(int size) {
	T set;
	assert(size >= 0);
	NEW0(set);
	if (size > 0)
		set->u.bytes = CALLOC(1, sizeof(unsigned)*nwords(size));
	set->size = size;
	return set;
}
void  Bit_free(T *set) {
	assert(set && *set);
	FREE(set);
}
int Bit_size(T set) {
	assert(set);
	return set->size;
}
int Bit_length(T set) {
	int length = 0, n;
	static char count[] = { 0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4 };
	assert(set);
	for (n = nbytes(set->size); --n >= 0; ) {
		unsigned char c = set->u.bytes[n];
		length += count[c&0xF] + count[c>>4];
	}
	return length;
}
int Bit_member(T set, int n) {
	assert(set);
	assert(0 <= n && n < set->size);
	return ((set->u.bytes[n/8]>>(n%8))&1);
}
void Bit_put(T set, int n) {
	assert(set);
	assert(0 <= n && n < set->size);
	set->u.bytes[n/8] |= 1<<(n%8);
}
void Bit_remove(T set, int n) {
	assert(set);
	assert(0 <= n && n < set->size);
	set->u.bytes[n/8] &= ~(1<<(n%8));
}
void Bit_set(T set, int lo, int hi) {
		assert(set);
		assert(0 <= lo && lo < set->size);
		assert(0 <= hi && hi < set->size);
		assert(lo <= hi);
	if (lo/8 < hi/8) {
		set->u.bytes[lo/8] |= msbmask[lo%8];
		{
			int i;
			for (i = lo/8+1; i < hi/8; i++)
				set->u.bytes[i] = 0xFF;
		}
		set->u.bytes[hi/8] |= lsbmask[hi%8];
	} else
		set->u.bytes[lo/8] |= (msbmask[lo%8]&lsbmask[hi%8]);
}
void Bit_clear(T set, int lo, int hi) {
		assert(set);
		assert(0 <= lo && lo < set->size);
		assert(0 <= hi && hi < set->size);
		assert(lo <= hi);
	if (lo/8 < hi/8) {
		int i;
		set->u.bytes[lo/8] &= ~msbmask[lo%8];
		for (i = lo/8+1; i < hi/8; i++)
			set->u.bytes[i] = 0;
		set->u.bytes[hi/8] &= ~lsbmask[hi%8];
	} else
		set->u.bytes[lo/8] &= ~(msbmask[lo%8]&lsbmask[hi%8]);
}
void Bit_not(T set, int lo, int hi) {
		assert(set);
		assert(0 <= lo && lo < set->size);
		assert(0 <= hi && hi < set->size);
		assert(lo <= hi);
	if (lo/8 < hi/8) {
		int i;
		set->u.bytes[lo/8] ^= msbmask[lo%8];
		for (i = lo/8+1; i < hi/8; i++)
			set->u.bytes[i] ^= 0xFF;
		set->u.bytes[hi/8] ^= lsbmask[hi%8];
	} else
		set->u.bytes[lo/8] ^= (msbmask[lo%8]&lsbmask[hi%8]);
}
int Bit_eq(T s, T t) {
	int i;
	assert(s && t);
	assert(s->size == t->size);
	for (i = 0; i < s->size; i++)
		if (s->u.words[i] != t->u.words[i])
			return 0;
	return 1;
}
int Bit_leq(T s, T t) {
	int i;
	assert(s && t);
	assert(s->size == t->size);
	for (i = 0; i < s->size; i++)
		if ((s->u.words[i]&~t->u.words[i]) != 0)
			return 0;
	return 1;
}
int Bit_lt(T s, T t) {
	int i, lt = 0;
	assert(s && t);
	assert(s->size == t->size);
	for (i = 0; i < s->size; i++)
		if ((s->u.words[i]&~t->u.words[i]) != 0)
			return 0;
		else if (s->u.words[i] != t->u.words[i])
			lt |= 1;
	return lt;
}
void Bit_map(T set,
	void apply(int n, int bit, void *cl), void *cl) {
	int n;
	assert(set);
	for (n = 0; n < set->size; n++)
		apply(n, ((set->u.bytes[n/8]>>(n%8))&1), cl);
}
T Bit_union(T s, T t) {
	assert( s ||  t);
	assert(!s || !t || s->size == t->size);
	if (s == NULL || s == t)
		return copy(t);
	else if (t == NULL)
		return copy(s);
	else {
		int i = s->size;
		T set = Bit_new(i);
		while (i-- > 0)
			set->u.words[i] = s->u.words[i] | t->u.words[i];
		return set;
	}
}
T Bit_inter(T s, T t) {
	assert( s ||  t);
	assert(!s || !t || s->size == t->size);
	if (s == NULL)
		return Bit_new(t->size);
	else if (t == NULL)
		return Bit_new(s->size);
	else {
		int i = s->size;
		T set = Bit_new(i);
		while (i-- > 0)
			set->u.words[i] = s->u.words[i] & t->u.words[i];
		return set;
	}
}
T Bit_minus(T s, T t) {
	assert( s ||  t);
	assert(!s || !t || s->size == t->size);
	if (s == NULL)
		return Bit_new(t->size);
	else if (t == NULL)
		return copy(s);
	else {
		int i = s->size;
		T set = Bit_new(i);
		while (i-- > 0)
			set->u.words[i] = s->u.words[i] ^ ~t->u.words[i];
		return set;
	}
}
T Bit_diff(T s, T t) {
	assert( s ||  t);
	assert(!s || !t || s->size == t->size);
	if (s == NULL)
		return Bit_new(t->size);
	else if (t == NULL)
		return copy(s);
	else {
		int i = s->size;
		T set = Bit_new(i);
		while (i-- > 0)
			set->u.words[i] = s->u.words[i] ^ t->u.words[i];
		return set;
	}
}
static char rcsid[] = "$RCSfile: RCS/bit.doc,v $ $Revision: 1.3 $";
