/*
This version of xref uses the Text, Array, and MP interfaces,
and stores the line numbers in 3-byte MP_Ts in Array_Ts.
It's slow, because it expands Array_Ts one element at a time.
getword picks off the identifiers right-to-left.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "assert.h"
#include "fmt.h"
#include "mem.h"
#include "mp.h"
#include "table.h"
#include "array.h"
#include "text.h"

static char rcsid[] = "$Id$";

Text_T getword(Text_T *line, Text_T first, Text_T rest) {
	int i, j;

	assert(line);
	if ((i = Text_rupto(*line, 1, 0, first)) > 0) {
		Text_T word = Text_sub(*line, j = Text_rmany(*line, 1, i + 1, rest), i + 1);
		*line = Text_sub(*line, 1, j);
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
		int j;
		Text_T *filename = array[i];
		if (filename->len > 0)
			Fmt_print("\t%T:", filename);
		for (j = 0; j < Array_length(array[i+1]); j++)
			Fmt_print(" %D", Array_get(array[i+1], j), 10);
		Fmt_print("\n");
		FREE(filename);
		Array_free((Array_T *)&array[i+1]);
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

void iref(char *name, FILE *fp, Table_T identifiers) {
	char buf[512];
	Text_T filename = { 0, "" };
	unsigned char linenum[3];

	if (name)
		filename = Text_put(name);
	MP_fromint(linenum, 1);
	while (fgets(buf, sizeof buf, fp) != NULL) {
		Text_T id, line = Text_put(buf);
		while ((id = getword(&line, first, rest)).len > 0) {
			Array_T array;
			Table_T files;
			files = Table_get(identifiers, &id);
			if (files == NULL) {
				files = Table_new(0, textcmp, texthash);
				Table_put(identifiers, copy(id), files);
			}
			array = Table_get(files, &filename);
			if (array == NULL) {
				array = Array_new(1, 3);
				Table_put(files, copy(filename), array);
				Array_put(array, 0, linenum);
			} else if (MP_cmp(Array_get(array, Array_length(array)-1), linenum) != 0) {
				Array_resize(array, Array_length(array) + 1);
				Array_put(array, Array_length(array) - 1, linenum);
			}
		}
		MP_addi(linenum, linenum, 1);
	}
}

int main(int argc, char *argv[]) {
	int i;
	Table_T identifiers = Table_new(10000, textcmp, texthash);
	Text_save_T mark = Text_save();

	Fmt_register('T', Text_fmt);
	Fmt_register('D', MP_fmt);
	MP_set(24);
	first = Text_cat(Text_cat(Text_ucase, Text_lcase), Text_box("_", 1));
	rest  = Text_cat(first, Text_digits);
	for (i = 1; i < argc; i++) {
		FILE *fp = fopen(argv[i], "r");
		if (fp == NULL)
			fprintf(stderr, "%s: can't open '%s' (%s)\n", argv[0], argv[i], strerror(errno));
		else {
			iref(argv[i], fp, identifiers);
			fclose(fp);
		}
	}
	if (argc == 1)
		iref(NULL, stdin, identifiers);
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
