/*
 * Thread creation and yielding test
 *
 * Tests the creation of multiples threads and the fact that a parent thread
 * should get returned to before its child is executed. The way the printing,
 * thread creation and yielding is done, the program should output:
 *
 * thread1
 * thread2
 * thread3
 */

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

int thread3(void* arg)
{
	printf("thread%d\n", uthread_self());
	return 3;
}

int thread2(void* arg)
{
	int retval;
	printf("thread%d\n", uthread_self());
	uthread_join(uthread_create(thread3, NULL), &retval);
	printf("Thread3 died with retval %d\n", retval);
	return 2;
}

int thread1(void* arg)
{
	int retval;
	printf("thread%d\n", uthread_self());
	uthread_join(uthread_create(thread2, NULL), &retval);
	printf("Thread2 died with retval %d\n", retval);
	return 1;
}

int main(void)
{
	int retval;
	uthread_join(uthread_create(thread1, NULL), &retval);
	printf("Thread1 died with retval %d\n", retval);
	return 0;
}
