/*
This version of ids uses Str and Seq,
and reads the identifers from right to left.
*/
#include <stdlib.h>
#include <string.h>
#include <mem.h>
#include "fmt.h"
#include "seq.h"
#include "str.h"

int main(int argc, char *argv[]) {
	char line[512];
	Seq_T seq = Seq_new(1);
	char *first = Str_catv("abcdefghijklmnopqrstuvwxyz", 1, 0,
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 1, 0, "_", 1, 0, NULL);
	char *rest = Str_cat("0123456789", 1, 0, first, 1, 0);

	Fmt_register('S', Str_fmt);
	while (fgets(line, sizeof line, stdin) != NULL) {
		int i, j = 0;
		while ((j = Str_rupto(line, 1, j, first)) > 0){
			i = Str_rmany(line, 1, j + 1, rest);
			Seq_addlo(seq, Str_sub(line, i, j + 1));
			j = i;
		}
		while (Seq_length(seq) > 0) {
			char *id = Seq_remlo(seq);
			Fmt_print("%S\n", id, 1, 0);
			FREE(id);
		}
	}
	FREE(first); FREE(rest);
	Seq_free(&seq);
	return EXIT_SUCCESS;
}
