#include <limits.h>
#include <stddef.h>
#include "mem.h"
#include "assert.h"
#include "table.h"
#define T Table_T
struct T {
	unsigned size;
	int (*cmp)(const void *x, const void *y);
	unsigned (*hash)(const void *key);
	int length;
	unsigned timestamp;
	struct binding {
		struct binding *link;
		const void *key;
		void *value;
	} **buckets;
};
static int cmpatom(const void *x, const void *y) {
	return x != y;
}
static unsigned hashatom(const void *key) {
	return (unsigned long)key>>2;
}
T Table_new(int hint, int cmp(const void *x, const void *y),
	unsigned hash(const void *key)) {
	T table;
	int i;
	static int primes[] = { 317, 317, 691, 1399, 2801,
		INT_MAX };
	assert(hint >= 0);
	for (i = 1; primes[i] < hint; i++)
		;
	table = CALLOC(1, sizeof *table +
		primes[i-1]*sizeof table->buckets[0]);
	table->size = primes[i-1];
	table->cmp  = cmp  ?  cmp : cmpatom;
	table->hash = hash ? hash : hashatom;
	table->buckets = (void *)(table + 1);
	return table;
}
void *Table_get(T table, const void *key) {
	int i;
	struct binding *p;
	assert(table);
	assert(key);
	i = (*table->hash)(key)%table->size;
	for (p = table->buckets[i]; p; p = p->link)
		if ((*table->cmp)(key, p->key) == 0)
			break;
	return p ? p->value : NULL;
}
void *Table_put(T table, const void *key, void *value) {
	int i;
	struct binding *p;
	assert(table);
	assert(key);
	i = (*table->hash)(key)%table->size;
	for (p = table->buckets[i]; p; p = p->link)
		if ((*table->cmp)(key, p->key) == 0)
			break;
	if (p == NULL) {
		NEW(p);
		p->key = key;
		p->link = table->buckets[i];
		table->buckets[i] = p;
		table->length++;
	}
	p->value = value;
	table->timestamp++;
	return value;
}
int Table_length(T table) {
	assert(table);
	return table->length;
}
void Table_map(T table,
	void apply(const void *key, void **value, void *cl),
	void *cl) {
	unsigned i, stamp;
	struct binding *p;
	assert(table);
	assert(apply);
	stamp = table->timestamp;
	for (i = 0; i < table->size; i++)
		for (p = table->buckets[i]; p; p = p->link) {
			apply(p->key, &p->value, cl);
			assert(table->timestamp == stamp);
		}
}
void *Table_remove(T table, const void *key) {
	int i;
	struct binding **pp;
	assert(table);
	assert(key);
	table->timestamp++;
	i = (*table->hash)(key)%table->size;
	for (pp = &table->buckets[i]; *pp; pp = &(*pp)->link)
		if ((*table->cmp)(key, (*pp)->key) == 0) {
			struct binding *p = *pp;
			void *value = p->value;
			*pp = p->link;
			FREE(&p);
			table->length--;
			return value;
		}
	return NULL;
}
void **Table_toArray(T table, void *end) {
	unsigned i, j = 0;
	void **array;
	struct binding *p;
	assert(table);
	array = ALLOC((2*table->length + 1)*sizeof (*array));
	for (i = 0; i < table->size; i++)
		for (p = table->buckets[i]; p; p = p->link) {
			array[j++] = (void *)p->key;
			array[j++] = p->value;
		}
	array[j] = end;
	return array;
}
void Table_free(T *table) {
	assert(table && *table);
	if (table && (*table)->length > 0) {
		unsigned i;
		struct binding *p, *q;
		for (i = 0; i < (*table)->size; i++)
			for (p = (*table)->buckets[i]; p; p = q) {
				q = p->link;
				FREE(&p);
			}
	}
	FREE(table);
}
static char rcsid[] = "$RCSfile: RCS/table.doc,v $ $Revision: 1.2 $";
