#include "assert.h"
#include "getword.h"

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
