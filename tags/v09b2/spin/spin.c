#include <stdio.h>
#include <stdlib.h>
#include "thread.h"
#include "sem.h"
struct args {
	Sem_T *mutex;
	int *ip;
};
int unsafe(void *cl) {
	int i, *ip = cl;
	for (i = 0; i < 30000; i++)
		*ip = *ip + 1;
	return EXIT_SUCCESS;
}
int safe(void *cl) {
	struct args *p = cl;
	int i;
	for (i = 0; i < 30000; i++)
		LOCK(*p->mutex)
			*p->ip = *p->ip + 1;
		END_LOCK;
	return EXIT_SUCCESS;
}
int n;
int main(int argc, char *argv[]) {
	int m = 5;
	Thread_init(1, NULL);
	if (argc >= 2)
		m = atoi(argv[1]);
	n = 0;
	{
		int i;
		for (i = 0; i < m; i++)
			Thread_new(unsafe, &n, 0, NULL);
		Thread_join(NULL);
	}
	printf("%d == %d\n", n, 30000*m);
	n = 0;
	{
		int i;
		struct args args;
		Sem_T mutex;
		Sem_init(&mutex, 1);
		args.mutex = &mutex;
		args.ip = &n;
		for (i = 0; i < m; i++)
			Thread_new(safe, &args, sizeof args, NULL);
		Thread_join(NULL);
	}
	printf("%d == %d\n", n, 30000*m);
	Thread_exit(EXIT_SUCCESS);
	return 0;
}
static char rcsid[] = "$RCSfile: RCS/thread.doc,v $ $Revision: 1.3 $";
