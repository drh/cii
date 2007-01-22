#include <stdio.h>
#include <stdlib.h>
#include "table.h"
#include <string.h>
#include "atom.h"
#include "set.h"
#include "mem.h"
#include "getword.h"
#include <ctype.h>
int compare(const char **, const char **);
void print(Table_T);
int cmpint(const int **, const int **);
void xref(char *, FILE *, Table_T);
int first(char);
int rest(char);
int intcmp(const int *, const int *);
unsigned inthash(const int *);
int linenum;
int main(int argc, char *argv[]) {
	int i;
	Table_T identifiers = Table_new(0, NULL, NULL);
	for (i = 1; i < argc; i++) {
		FILE *fp = fopen(argv[i], "r");
		if (fp == NULL)
			fprintf(stderr, "%s: can't open '%s'\n", argv[0],
				argv[i]);
		else {
			xref(argv[i], fp, identifiers);
			fclose(fp);
		}
	}
	if (argc == 1)
		xref(NULL, stdin, identifiers);
	{
		int i;
		void **array = Table_toArray(identifiers, NULL);
		qsort(array, Table_length(identifiers), 2*sizeof *array,
			(int (*)(const void *, const void *))compare);
		for (i = 0; array[i]; i += 2) {
			printf("%s", array[i]);
			print(array[i+1]);
		}
		FREE(&array);
	}
	return EXIT_SUCCESS;
}
int compare(const char **x, const char **y) {
	return strcmp(*x, *y);
}
void print(Table_T files) {
	int i;
	void **array = Table_toArray(files, NULL);
	qsort(array, Table_length(files), 2*sizeof *array,
		(int (*)(const void *, const void *))compare);
	for (i = 0; array[i]; i += 2) {
		if (*(char **)array[i] != NULL)
			printf("\t%s:", array[i]);
		{
			int j;
			void **lines = Set_toArray(array[i+1], NULL);
			qsort(lines, Set_length(array[i+1]), sizeof *lines,
				(int (*)(const void *, const void *))cmpint);
			for (j = 0; lines[j]; j++)
				printf(" %d", *(int *)lines[j]);
			FREE(&lines);
		}
		printf("\n");
	}
	FREE(&array);
}
int cmpint(const int **x, const int **y) {
	if (**x < **y)
		return -1;
	else if (**x > **y)
		return +1;
	else
		return 0;
}
void xref(char *name, FILE *fp, Table_T identifiers) {
	char buf[128];
	if (name == NULL)
		name = "";
	name = Atom_string(name);
	linenum = 1;
	while (getword(fp, buf, sizeof buf, first, rest)) {
		Set_T set;
		Table_T files;
		char *id = Atom_string(buf);
		files = Table_get(identifiers, id);
		if (files == NULL) {
			files = Table_new(0, NULL, NULL);
			Table_put(identifiers, id, files);
		}
		set = Table_get(files, name);
		if (set == NULL) {
			set = Set_new(0,
				(int (*)(const void *, const void *))intcmp,
				(unsigned (*)(const void *))inthash);
			Table_put(files, name, set);
		}
		{
			int *p = &linenum;
			if (!Set_member(set, p)) {
				NEW(p);
				*p = linenum;
				Set_put(set, p);
			}
		}
	}
}
int first(char c) {
	if (c == '\n')
		linenum++;
	return isalpha(c) || c == '_';
}
int rest(char c) {
	return isalpha(c) || c == '_' || isdigit(c);
}
int intcmp(const int *x, const int *y) {
	if (*x < *y)
		return -1;
	else if (*x > *y)
		return +1;
	else
		return 0;
}
unsigned inthash(const int *x) {
	return *x;
}
static char rcsid[] = "$RCSfile: RCS/set.doc,v $ $Revision: 1.2 $";
