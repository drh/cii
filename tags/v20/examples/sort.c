static char rcsid[] = "$Id$" "\n$Id$";
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "assert.h"
#include "fmt.h"
#include "thread.h"
#include "mem.h"
struct args {
	int *a;
	int lb, ub;
};
int cutoff = 10000;
#include <stdarg.h>
#define Fmt_print outs

static void outs(const char *fmt, ...) {
	char *buf;
	va_list_box box;

	va_start(box.ap, fmt);
	buf = Fmt_vstring(fmt, &box);
	va_end(box.ap);
	printf("%s", buf);
	FREE(buf);
}

int partition(int a[], int i, int j) {
	int v, k, t;
	j++;
	k = i;
	v = a[k];
	while (i < j) {
		i++; while (a[i] < v && i < j) i++;
		j--; while (a[j] > v         ) j--;
		if (i < j) { t = a[i]; a[i] = a[j]; a[j] = t; }
	}
	t = a[k]; a[k] = a[j]; a[j] = t;
	return j;
}
int quick(void *cl) {
	struct args *p = cl;
	int lb = p->lb, ub = p->ub;
	if (lb < ub) {
		int k = partition(p->a, lb, ub);
		p->lb = lb;
		p->ub = k - 1;
		if (k - lb > cutoff) {
			Thread_T t;
			t = Thread_new(quick, p, sizeof *p, NULL);
			Fmt_print("thread %p sorted %d..%d\n", t, lb, k - 1);
		} else
			quick(p);
		p->lb = k + 1;
		p->ub = ub;
		if (ub - k > cutoff) {
			Thread_T t;
			t = Thread_new(quick, p, sizeof *p, NULL);
			Fmt_print("thread %p sorted %d..%d\n", t, k + 1, ub);
		} else
			quick(p);
	}
	return EXIT_SUCCESS;
}
void sort(int *x, int n, int argc, char *argv[]) {
	struct args args;
	if (argc >= 3)
		cutoff = atoi(argv[2]);
	args.a = x;
	args.lb = 0;
	args.ub = n - 1;
	quick(&args);
	Thread_join(NULL);
}
main(int argc, char *argv[]) {
	int i, n = 100000, *x, preempt;
	preempt = Thread_init(1, NULL);
	assert(preempt == 1);
	if (argc >= 2)
		n = atoi(argv[1]);
	x = CALLOC(n, sizeof (int));
	srand(time(NULL));
	for (i = 0; i < n; i++)
		x[i] = rand();
	sort(x, n, argc, argv);
	for (i = 1; i < n; i++)
		if (x[i] < x[i-1])
			break;
	assert(i == n);
	Thread_exit(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}
