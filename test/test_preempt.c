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
	uthread_create(thread3, NULL);
	printf("thread%d\n", uthread_self());
	return 2;
}

int thread1(void* arg)
{
	uthread_create(thread2, NULL);
	printf("thread%d\n", uthread_self());
	while(1);
	return 1;
}

int main(void)
{
	uthread_join(uthread_create(thread1, NULL), NULL);
	return 0;
}
