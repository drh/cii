/*
This version of xref uses the Text and Ring interfaces.
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "assert.h"
#include "fmt.h"
#include "mem.h"
#include "table.h"
#include "integer.h"
#include "ring.h"
#include "text.h"

static char rcsid[] = "$Id$";

Text_T getword(Text_T *line, Text_T first, Text_T rest) {
	int i, j;

	assert(line);
	if ((i = Text_upto(*line, 1, 0, first)) > 0) {
		Text_T word = Text_sub(*line, j = Text_many(*line, i, 0, rest), i);
		*line = Text_sub(*line, j, 0);
		return word;
	} else
		return Text_null;
}

Text_T first, rest;

int textcmp(const void *x, const void *y) {
	return Text_cmp(*(Text_T *)x, *(Text_T *)y);
}

int compare(const void *x, const void *y) {
	return textcmp(*(Text_T **)x, *(Text_T **)y);
}

unsigned texthash(const void *x) {
	int i;
	unsigned h = 0;
	const Text_T *t = x;

	for (i = 0; i < t->len; i++)
		h = (h<<1) + t->str[i];
	return h;
}

void print(Table_T files) {
	int i;

	void **array = Table_toArray(files, NULL);
	qsort(array, Table_length(files), 2*sizeof (*array), compare);
	for (i = 0; array[i]; i += 2) {
		Ring_T ring = array[i+1];
		Text_T *filename = array[i];
		if (filename->len > 0)
			Fmt_print("\t%T:", filename);
		while (Ring_length(ring) > 0) {
			Integer_T line = Ring_remhi(ring);
			Fmt_print(" %D", line);
			FREE(line);
		}
		Fmt_print("\n");
		FREE(filename);
		Ring_free(&ring);
	}
	FREE(array);
	Table_free(&files);
}

Text_T *copy(Text_T t) {
	Text_T *p;

	NEW(p);
	*p = t;
	return p;
}

void cref(char *name, FILE *fp, Table_T identifiers) {
	char buf[512];
	Text_T filename = { 0, "" };
	int linenum;

	if (name)
		filename = Text_put(name);
	for (linenum = 1; fgets(buf, sizeof buf, fp) != NULL; linenum++) {
		Text_T id, line = Text_put(buf);
		while ((id = getword(&line, first, rest)).len > 0) {
			Ring_T ring;
			Table_T files;
			files = Table_get(identifiers, &id);
			if (files == NULL) {
				files = Table_new(0, textcmp, texthash);
				Table_put(identifiers, copy(id), files);
			}
			ring = Table_get(files, &filename);
			if (ring == NULL) {
				ring = Ring_new();
				Table_put(files, copy(filename), ring);
				Ring_addlo(ring, Integer_new(linenum));
			} else if (Integer_get(Ring_get(ring, 0)) != linenum)
				Ring_addlo(ring, Integer_new(linenum));
		}
	}
}

int main(int argc, char *argv[]) {
	int i;
	Table_T identifiers = Table_new(10000, textcmp, texthash);
	Text_save_T mark = Text_save();

	Fmt_register('T', Text_fmt);
	Fmt_register('D', Integer_fmt);
	first = Text_cat(Text_cat(Text_ucase, Text_lcase), Text_box("_", 1));
	rest  = Text_cat(first, Text_digits);
	for (i = 1; i < argc; i++) {
		FILE *fp = fopen(argv[i], "r");
		if (fp == NULL)
		fprintf(stderr, "%s: can't open '%s' (%s)\n", argv[0], argv[i], strerror(errno));
		else {
			cref(argv[i], fp, identifiers);
			fclose(fp);
		}
	}
	if (argc == 1)
		cref(NULL, stdin, identifiers);
	{
		int i;
		void **array = Table_toArray(identifiers, NULL);
		qsort(array, Table_length(identifiers), 2*sizeof (*array), compare);
		for (i = 0; array[i]; i += 2) {
			Fmt_print("%T", array[i]);
			print(array[i+1]);
			FREE(array[i]);
		}
		FREE(array);
		Table_free(&identifiers);
	}
	Text_restore(&mark);
	return EXIT_SUCCESS;
}
