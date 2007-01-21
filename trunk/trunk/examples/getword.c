static char rcsid[] = "$Id: H:/drh/idioms/book/RCS/table.doc,v 1.13 1997/10/27 23:10:11 drh Exp $";
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "assert.h"
#include "getword.h"
int getword(FILE *fp, char *buf, int size,
	int first(int c), int rest(int c)) {
	int i = 0, c;
	assert(fp && buf && size > 1 && first && rest);
	c = getc(fp);
	for ( ; c != EOF; c = getc(fp))
		if (first(c)) {
			{
				if (i < size - 1)
					buf[i++] = c;
			}
			c = getc(fp);
			break;
		}
	for ( ; c != EOF && rest(c); c = getc(fp))
		{
			if (i < size - 1)
				buf[i++] = c;
		}
	if (i < size)
		buf[i] = '\0';
	else
		buf[size-1] = '\0';
	if (c != EOF)
		ungetc(c, fp);
	return i > 0;
}
