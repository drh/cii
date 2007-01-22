#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
int linenum;
int getword(FILE *, char *, unsigned);
void doubleword(char *, FILE *);
int main(int argc, char *argv[]) {
	int i;
	for (i = 1; i < argc; i++) {
		FILE *fp = fopen(argv[i], "r");
		if (fp == NULL)
			fprintf(stderr, "%s: can't open '%s'\n", argv[0],
				argv[i]);
		else {
			doubleword(argv[i], fp);
			fclose(fp);
		}
	}
	if (argc == 1)
		doubleword(NULL, stdin);
	return EXIT_SUCCESS;
}
int getword(FILE *fp, char *buf, unsigned size) {
	int c;
	c = getc(fp);
	for ( ; c != EOF && isspace(c); c = getc(fp))
		if (c == '\n')
			linenum++;
	{
		int i = 0;
		for ( ; c != EOF && !isspace(c); c = getc(fp))
			if (i < size - 1)
				buf[i++] = tolower(c);
		if (i < size)
			buf[i] = '\0';
	}
	if (c != EOF)
		ungetc(c, fp);
	return buf[0] != '\0';
}
void doubleword(char *name, FILE *fp) {
	char prev[128], word[128];
	linenum = 1;
	prev[0] = 0;
	while (getword(fp, word, sizeof word)) {
		if (isalpha(word[0]) && strcmp(prev, word) == 0)
			{
				if (name != NULL)
					printf("%s:", name);
				printf("%d: %s\n", linenum, word);
			}
		strcpy(prev, word);
	}
}
static char rcsid[] = "$RCSfile: RCS/intro.doc,v $ $Revision: 1.1 $";
