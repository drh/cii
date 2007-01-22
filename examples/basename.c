#include <stdlib.h>
#include "fmt.h"
#include "str.h"

char *dirname(char *path, int i, int j) {
	if ((j = Str_rchr(path, i, j, '/')) > 0)
		return Str_dup(path, i, j + 1, 1);
	else
		return Str_dup("./", 1, 0, 1);
}

char *basename(char *path, int i, int j, const char *suffix) {
	i = Str_rchr(path, i, j, '/');
	j = Str_rmatch(path, i + 1, 0, suffix);
	return Str_dup(path, i + 1, j, 1);
}

int main(int argc, char *argv[]) {
	int i;

	if (Str_find(argv[0], 1, 0, "dirname") > 0)
		for (i = 1; i < argc; i += 2)
			Fmt_print("%s\n", dirname(argv[i], 1, 0));
	else
		for (i = 1; i < argc; i += 2)
			Fmt_print("%s\n", basename(argv[i], 1, 0, i+1 < argc ? argv[i+1] : ""));
	return EXIT_SUCCESS;
}
