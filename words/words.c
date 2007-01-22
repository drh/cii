/*
This version of ids uses Text and Ring,
and reads the identifiers from right to left.
This version is a circuitous solution to Exercise 16.1.
*/
#include <stdlib.h>
#include <stdio.h>
#include "fmt.h"
#include "ring.h"
#include "text.h"
#include "mem.h"

int main(int argc, char *argv[]) {
	char buf[512];
	Text_save_T save = Text_save();
	Ring_T ring = Ring_new();
	Text_T first = Text_cat(Text_cat(Text_ucase, Text_lcase), Text_box("_", 1));
	Text_T rest  = Text_cat(first, Text_digits);

	Fmt_register('T', Text_fmt);
	while (fgets(buf, sizeof buf, stdin) != NULL) {
		Text_T line = Text_put(buf), *id;
		int j = 0;
		while ((j = Text_rupto(line, 1, j, first)) > 0) {
			int i = Text_rmany(line, 1, j + 1, rest);
			NEW(id);
			*id = Text_sub(line, i, j + 1);
			Ring_addlo(ring, id);
			j = i;
		}
		while (Ring_length(ring) > 0) {
			id = Ring_remlo(ring);
			Fmt_print("%T\n", id);
			FREE(id);
		}

	}
	Ring_free(&ring);
	Text_restore(&save);
	return EXIT_SUCCESS;
}
