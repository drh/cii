#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <windows.h>
#include <process.h>
#include "assert.h"
#include "mem.h"
#include "thread.h"
#include "sem.h"

static char rcsid[] = "$Id$";

#define T Thread_T
struct T {
	DWORD IDThread;		/* Win 32 thread identifier */
	T handle;		/* self pointer */
	int code;		/* exit code */
	HANDLE join;		/* join semaphore */
	T joinlist;		/* threads waiting on join */
	T link;			/* next thread on this join list */
	T next;			/* next thread on this hash chain */
	int alerted;		/* 1 if this thread has been alerted */
	int (*apply)(void *);	/* initial function for this thread */
	void *args;		/* argument for apply */
};

const Except_T Thread_Alerted = { "Thread alerted" };
const Except_T Thread_Failed  = { "Thread creation failed" };

static T allthreads[317];
#define HASH(id) ((int)((id)%(sizeof allthreads/sizeof allthreads[0])))
static int nthreads;		/* number of threads in allthreads */
static T root;
static HANDLE join0;		/* Thread_join(NULL) semaphore */
static int join0count;		/* number of threads waiting on join0; always 0 or 1 */

static int critical;
static CRITICAL_SECTION csection;
#define ENTERCRITICAL EnterCriticalSection(&csection); assert(critical == 0); critical++
#define LEAVECRITICAL   critical--; assert(critical == 0); LeaveCriticalSection(&csection)

static T getThreadByID(DWORD id) {
	T t;

	ENTERCRITICAL;
	for (t = allthreads[HASH(id)]; t != NULL; t = t->next)
		if (t->IDThread == id)
			break;
	LEAVECRITICAL;
	assert(t);
	return t;
}

/* removeThread - must be called from within a critical region */
static void removeThread(T t) {
	T *q;

	q = &allthreads[HASH(t->IDThread)];
	for ( ; *q != NULL && *q != t; q = &(*q)->next)
		;
	assert(*q == t);
	*q = t->next;
	nthreads--;
	t->handle = NULL;
}

static void addThread(T t) {
	T *q;

	ENTERCRITICAL;
	q = &allthreads[HASH(t->IDThread)];
	t->next = *q;
	*q = t;
	nthreads++;
	t->handle = t;
	LEAVECRITICAL;
}

static void testalert(T t) {
	ENTERCRITICAL;
	if (t->alerted) {
		t->alerted = 0;
		LEAVECRITICAL;
		RAISE(Thread_Alerted);
	}
	LEAVECRITICAL;
}

int Thread_init(int preempt, ...) {
	assert(preempt == 0 || preempt == 1);
	assert(root == NULL);
	TRY
		NEW0(root);
	EXCEPT(Mem_Failed)
		return -1;
	END_TRY;
	join0 = CreateSemaphore(NULL, 0, 1, NULL);
	if (join0 == NULL)
		return -1;
	root->join = CreateSemaphore(NULL, 0, INT_MAX, NULL);
	if (root->join == NULL) {
		BOOL result = CloseHandle(join0);
		assert(result == TRUE);
		return -1;
	}
	InitializeCriticalSection(&csection);
	root->IDThread = GetCurrentThreadId();
	addThread(root);
	/* handle preempt == 0 */
	return 1;
}

T Thread_self(void) {
	assert(root);
	return getThreadByID(GetCurrentThreadId());
}

void Thread_pause(void) {
	assert(root);
	Sleep(0);
}

int Thread_join(T t) {
	T current = Thread_self();

	assert(root);
	assert(t != current);
	testalert(current);
	if (t != NULL) {
		ENTERCRITICAL;
		if (t->handle == t) {
			HANDLE join = t->join;
			DWORD result;
			assert(current->link == NULL);
			current->link = t->joinlist;
			t->joinlist = current;
			LEAVECRITICAL;
			result = WaitForSingleObject(join, INFINITE);
			assert(result != WAIT_FAILED);
			testalert(current);
			return current->code;
		} else {
			LEAVECRITICAL;
			return -1;
		}
	}
	ENTERCRITICAL;
	if (nthreads > 1) {
		DWORD result;
		assert(join0count == 0);
		join0count++;
		LEAVECRITICAL;
		result = WaitForSingleObject(join0, INFINITE);
		assert(result != WAIT_FAILED);
		ENTERCRITICAL;
		join0count--;
		LEAVECRITICAL;
		testalert(current);
	} else {
		assert(join0count == 0);
		LEAVECRITICAL;
		return 0;
	}
}

void Thread_exit(int code) {
	BOOL result;
	T current = Thread_self();

	ENTERCRITICAL;
	removeThread(current);
	if (current->joinlist != NULL) {
		T t, n;
		int count = 0;
		assert(current->join);
		for (t = current->joinlist; t != NULL; t = n) {
			t->code = code;
			n = t->link;
			t->link = NULL;
			count++;
		}
		current->joinlist = NULL;
		result = ReleaseSemaphore(current->join, count, NULL);
		assert(result == TRUE);
	}
	result = CloseHandle(current->join);
	assert(result == TRUE);
	current->join = NULL;
	if (join0count > 0 && nthreads == 1) {
		assert(join0count == 1);
		result = ReleaseSemaphore(join0, 1, NULL);
		assert(result == TRUE);
	}
	if (nthreads == 0) {
		result = CloseHandle(join0);
		assert(result == TRUE);
	}
	FREE(current);
	LEAVECRITICAL;
	_endthreadex(code);
}

void Thread_alert(T t) {
	assert(root);
	ENTERCRITICAL;
	assert(t && t->handle == t);
	t->alerted = 1;
	LEAVECRITICAL;
}

static unsigned __stdcall start(void *p) {
	T t = p;

	if (Except_index == -1)
		Except_init();
	TlsSetValue(Except_index, NULL);
	Thread_exit((*t->apply)(t->args));
	return 0;
}

T Thread_new(int apply(void *), void *args, int nbytes, ...) {
	T t;
	HANDLE hThread;

	assert(root);
	assert(apply);
	assert(args && nbytes >= 0 || args == NULL);
	if (args == NULL)
		nbytes = 0;
	TRY
		t = ALLOC((sizeof (*t) + nbytes + 15)&~15);
		memset(t, '\0', sizeof *t);
	EXCEPT(Mem_Failed)
		RAISE(Thread_Failed);
	END_TRY;
	t->join = CreateSemaphore(NULL, 0, INT_MAX, NULL);
	if (t->join == NULL) {
		FREE(t);
		RAISE(Thread_Failed);
	}
	if (nbytes > 0) {
		t->args = t + 1;
		memcpy(t->args, args, nbytes);
	} else
		t->args = args;
	t->apply = apply;
	hThread = (HANDLE)_beginthreadex(
		NULL,		/* default security attributes */
		0,		/* default stack size */
		start,		/* initial function */
		t,		/* start's argument */
		0,		/* default thread creation flags */
		&t->IDThread	/* where to store the thread id */
		);
	if (hThread == NULL) {
		CloseHandle(t->join);
		FREE(t);
		RAISE(Thread_Failed);
	}
	CloseHandle(hThread);
	addThread(t);
	return t;
}
#undef T

#define T Sem_T
T *Sem_new(int count) {
	T *s;

	NEW(s);
	Sem_init(s, count);
	return s;
}

void Sem_init(T *s, int count) {
	assert(root);
	assert(s);
	assert(count >= 0);
	s->count = 0;
	s->queue = CreateSemaphore(NULL, count, INT_MAX, NULL);
	assert(s->queue);
}

void Sem_wait(T *s) {
	DWORD result;
	Thread_T current = Thread_self();

	assert(s);
	testalert(current);
	result = WaitForSingleObject(s->queue, INFINITE);
	assert(result != WAIT_FAILED);
	ENTERCRITICAL;
	if (current->alerted) {
		BOOL result;
		current->alerted = 0;
		LEAVECRITICAL;
		result = ReleaseSemaphore(s->queue, 1, NULL);
		assert(result == TRUE);
		RAISE(Thread_Alerted);
	}
	LEAVECRITICAL;
}

void Sem_signal(T *s) {
	BOOL result;

	assert(root);
	assert(s);
	result = ReleaseSemaphore(s->queue, 1, NULL);
	assert(result == TRUE);
}
#undef T
