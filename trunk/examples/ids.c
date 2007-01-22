static char rcsid[] = "$Id$";
#include <stdlib.h>
#include <stdio.h>
#include "fmt.h"
#include "str.h"
int main(int argc, char *argv[]) {
	char line[512];
	static char set[] = "0123456789_"
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	Fmt_register('S', Str_fmt);
	while (fgets(line, sizeof line, stdin) != NULL) {
		int i = 1, j;
		while ((i = Str_upto(line, i, 0, &set[10])) > 0){
			j = Str_many(line, i, 0, set);
			Fmt_print("%S\n", line, i, j);
			i = j;
		}
	}
	return EXIT_SUCCESS;
}
