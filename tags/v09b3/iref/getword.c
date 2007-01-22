#include "assert.h"
#include "getword.h"

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
