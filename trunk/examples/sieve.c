static const char *rcsid = "$Id$" "\n$Id$";
#include <stdio.h>
#include <stdlib.h>
#include "assert.h"
#include "fmt.h"
#include "thread.h"
#include "chan.h"
struct args {
	Chan_T c;
	int n, last;
};
int source(void *cl) {
	struct args *p = cl;
	int i = 2;
	if (Chan_send(p->c, &i, sizeof i))
		for (i = 3; Chan_send(p->c, &i, sizeof i); )
			i += 2;
	return EXIT_SUCCESS;
}
void filter(int primes[], Chan_T input, Chan_T output) {
	int j, x;
	for (;;) {
		Chan_receive(input, &x, sizeof x);
		for (j = 0; primes[j] != 0 && x%primes[j] != 0; j++)
			;
		if (primes[j] == 0)
			 if (Chan_send(output, &x, sizeof x) == 0)
				break;
	}
	Chan_receive(input, &x, 0);
}
int sink(void *cl) {
	struct args *p = cl;
	Chan_T input = p->c;
	int i = 0, j, x, primes[256];
	primes[0] = 0;
	for (;;) {
		Chan_receive(input, &x, sizeof x);
 		for (j = 0; primes[j] != 0 && x%primes[j] != 0; j++)
			;
		if (primes[j] == 0) {
			if (x > p->last)
				break;
			Fmt_print(" %d", x);
			primes[i++] = x;
			primes[i] = 0;
			if (i == p->n)
				{
					p->c = Chan_new();
					Thread_new(sink, p, sizeof *p, NULL);
					filter(primes, input, p->c);
					return EXIT_SUCCESS;
				}
		}
	}
	Fmt_print("\n");
	Chan_receive(input, &x, 0);
 	return EXIT_SUCCESS;
}
int main(int argc, char *argv[]) {
	struct args args;
	Thread_init(1, NULL);
	args.c = Chan_new();
	Thread_new(source, &args, sizeof args, NULL);
	args.n    = argc > 2 ? atoi(argv[2]) : 5;
	args.last = argc > 1 ? atoi(argv[1]) : 1000;
	Thread_new(sink,   &args, sizeof args, NULL);
	Thread_exit(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}
