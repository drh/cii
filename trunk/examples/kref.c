/*
This version of xref, which is derived from cref, uses the Str and Seq
interfaces, and it uses Atom_Ts are used for the line numbers.
This version is a possible solution to exercise 11.5.
Also, this version of getword picks off the identifiers from
right to left.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "assert.h"
#include "atom.h"
#include "fmt.h"
#include "mem.h"
#include "table.h"
#include "seq.h"
#include "str.h"

static char rcsid[] = "$Id$";

int getword(char *line, int *i, char *first, char *rest) {
	int j;

	assert(line);
	assert(i);
	assert(first);
	assert(rest);
	if ((j = Str_rupto(line, 1, *i, first)) > 0)
		j = Str_rmany(line, 1, *i = j + 1, rest);
	return j;
}

char *first, *rest;

int compare(const void *x, const void *y) {
	return Str_cmp(*(char **)x, 1, 0, *(char **)y, 1, 0);
}

unsigned strhash(const void *x) {
	const char *str = x;
	unsigned h = 0;

	while (*str)
		h = (h<<1) + *str++;
	return h;
}

void print(Table_T files) {
	int i;

	void **array = Table_toArray(files, NULL);
	qsort(array, Table_length(files), 2*sizeof (*array), compare);
	for (i = 0; array[i]; i += 2) {
		Seq_T seq = array[i+1];
		char *filename = array[i];
		if (*filename)
			Fmt_print("\t%S:", filename, 1, 0);
		while (Seq_length(seq) > 0)
			Fmt_print(" %S", Seq_remhi(seq), 1, 0);
		Fmt_print("\n");
		FREE(filename);
		Seq_free(&seq);
	}
	FREE(array);
	Table_free(&files);
}

void kref(char *name, FILE *fp, Table_T identifiers) {
	char buf[512];
	const char *filename = "";
	int linenum;

	if (name)
		filename = name;
	for (linenum = 1; fgets(buf, sizeof buf, fp) != NULL; linenum++) {
		int i, j;
		for (i = 0; (j = getword(buf, &i, first, rest)) > 0; i = j) {
			char *id = Str_sub(buf, j, i);
			const char *ln = Atom_int(linenum);
			Seq_T seq;
			Table_T files;
			files = Table_get(identifiers, id);
			if (files == NULL) {
				files = Table_new(0,
					(int (*)(const void *, const void *))strcmp, strhash);
				Table_put(identifiers, id, files);
			} else
				FREE(id);
			seq = Table_get(files, filename);
			if (seq == NULL) {
				seq = Seq_new(0);
				Table_put(files, Str_dup(filename, 1, 0, 1), seq);
				Seq_addlo(seq, (void *)ln);
			} else if (Seq_get(seq, 0) != ln)
				Seq_addlo(seq, (void *)ln);
		}
	}
}

int main(int argc, char *argv[]) {
	int i;
	Table_T identifiers = Table_new(5000,
		(int (*)(const void *, const void *))strcmp, strhash);

	Fmt_register('S', Str_fmt);
	first = Str_catv("abcdefghijklmnopqrstuvwxyz", 1, 0,
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 1, 0, "_", 1, 0, NULL);
	rest  = Str_cat(first, 1, 0, "0123456789", 1, 0);
	for (i = 1; i < argc; i++) {
		FILE *fp = fopen(argv[i], "r");
		if (fp == NULL)
			fprintf(stderr, "%s: can't open '%s' (%s)\n", argv[0], argv[i], strerror(errno));
		else {
			kref(argv[i], fp, identifiers);
			fclose(fp);
		}
	}
	if (argc == 1)
		kref(NULL, stdin, identifiers);
	{
		int i;
		void **array = Table_toArray(identifiers, NULL);
		qsort(array, Table_length(identifiers), 2*sizeof (*array), compare);
		for (i = 0; array[i]; i += 2) {
			Fmt_print("%S", array[i], 1, 0);
			print(array[i+1]);
			FREE(array[i]);
		}
		FREE(array);
		Table_free(&identifiers);
	}
	return EXIT_SUCCESS;
}
