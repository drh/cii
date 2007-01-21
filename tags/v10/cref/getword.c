#include "assert.h"
#include "getword.h"

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
