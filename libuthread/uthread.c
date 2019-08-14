#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <limits.h>

#include "context.h"
#include "preempt.h"
#include "queue.h"
#include "uthread.h"

#define READY 0
#define RUNNING 1
#define BLOCKED 2
#define ZOMBIE -1

queue_t ready_queue = NULL;
queue_t running_queue = NULL;
queue_t blocked_queue = NULL;

uthread_t tid_num = 0;

typedef struct thread* thread_t;

struct thread{
	int state, p_flag, retval;
	uthread_t tid;
	uthread_ctx_t ctx;
	char *stack;
	uthread_t tid_p;
};

static int find_tid(void *data, void *arg)
{
	thread_t a = (thread_t) data;
	int match = (int)(long) arg;

	if(a->tid == match)
		return 1;

	return 0;
}

static int find_state(void *data, void *arg)
{
	thread_t a = (thread_t) data;
	int match = (int)(long) arg;

	if(a->state == match)
		return 1;

	return 0;
}

void uthread_yield(void)
{
	preempt_disable();
	thread_t thread_c, thread_n;

	//Puts current running thread to ready
	queue_dequeue(running_queue, (void**) &thread_c);
	thread_c->state = READY;
	queue_enqueue(ready_queue, thread_c);

	//Puts next ready thread to running
	queue_dequeue(ready_queue, (void**) &thread_n);
	thread_n->state = RUNNING;
	queue_enqueue(running_queue, thread_n);
	
	//Context Switches
	uthread_ctx_switch(&(thread_c->ctx), &(thread_n->ctx));
	preempt_enable();
}

uthread_t uthread_self(void)
{
	preempt_disable();
	thread_t temp;

	//Finds the runnng_queue and returns its tid
	queue_dequeue(running_queue, (void**) &temp);
	queue_enqueue(running_queue, temp);
	preempt_enable();
	return temp->tid;	
}

int main_thread(uthread_func_t func, void **arg)
{
	thread_t main_t = (thread_t) malloc(sizeof(thread_t));
	void* stack_top = uthread_ctx_alloc_stack();

	if(main_t == NULL)
		return -1;

	//Initializes main thread and all queues
	ready_queue = queue_create();
	running_queue = queue_create();
	blocked_queue = queue_create();
	uthread_ctx_init(&(main_t->ctx), stack_top, func, arg);
	main_t->state = RUNNING;
	main_t->tid = tid_num++;
	main_t->tid_p = -1;
	main_t->p_flag = 0;
	main_t->retval = -1;
	main_t->stack = stack_top;
	queue_enqueue(running_queue, main_t);
	
	//Starts preempt but disables it first
	preempt_start();
	preempt_disable();
	return 0;
}

int uthread_create(uthread_func_t func, void *arg)
{
	preempt_disable();
	thread_t temp = (thread_t) malloc(sizeof(thread_t));
	void* stack_top = uthread_ctx_alloc_stack();

	if(running_queue == NULL)
		main_thread(func, &arg);

	if(tid_num > USHRT_MAX)
		return -1;

	//Initializes thread	
	uthread_ctx_init(&(temp->ctx), stack_top, func, arg);
	temp->state = READY;
	temp->tid = tid_num++;
	temp->tid_p = -1;
	temp->p_flag = 0;
	temp->retval = -1;
	temp->stack = stack_top;
	queue_enqueue(ready_queue, temp);

	preempt_enable();
	return temp->tid;
}

void uthread_exit(int retval)
{
	preempt_disable();
	thread_t thread_c, thread_n, temp;
	
	queue_dequeue(running_queue, (void**) &thread_c);
	queue_enqueue(blocked_queue, thread_c);
	thread_c->state = ZOMBIE;

	//If the parent thread is Blocked
	//Enqueue back into the ready queue
	if(thread_c->p_flag == 1){
		queue_iterate(blocked_queue, find_tid, 
				(void*)(long) thread_c->tid_p, 
				(void**) &temp);
		queue_enqueue(ready_queue, temp);
		temp->state = READY;
		temp->retval = retval;
	}

	//Finds next ready thread and set it to running
	queue_dequeue(ready_queue, (void**) &thread_n);
	queue_enqueue(running_queue, thread_n);
	thread_n->state = RUNNING;
	uthread_join(thread_n->tid, NULL);	
	preempt_enable();
	uthread_ctx_switch(&(thread_c->ctx), &(thread_n->ctx));
}

int uthread_join(uthread_t tid, int *retval)
{
	preempt_disable();
	uthread_t tid_c = uthread_self();
	thread_t thread_c, thread_n, thread_z, temp;

	if(tid <= 0 || tid == tid_c || tid > tid_num)
		return -1;
	
	queue_iterate(ready_queue, find_tid, 
			(void*)(long) tid, (void**)&temp);

	if(temp->p_flag == 1)
		return -1;

	//Dequeues running thread
	//Then finds the next ready and blocked/zombie thread
	queue_dequeue(running_queue, (void**) &thread_c);
	queue_iterate(ready_queue, find_state, (void*) READY, (void**) &thread_n);
	queue_iterate(blocked_queue, find_tid, 
			(void*)(long) tid, 
			(void**)&thread_z);

	//Checks if next thread is ready and thread_z is not a zombie
	//Blocks current thread and sets next thread to running
	if(thread_n->state == READY && thread_z->state != ZOMBIE){
		queue_dequeue(running_queue, (void**) &thread_c);
		queue_dequeue(ready_queue, (void**) &thread_n);
		thread_c->state = BLOCKED;
		thread_n->state = RUNNING;
		queue_enqueue(blocked_queue, thread_c);
		queue_enqueue(running_queue, thread_n);
		thread_n->tid_p = thread_c->tid;
		thread_n->p_flag = 1;
		uthread_ctx_switch(&(thread_c->ctx), &(thread_n->ctx));	
		
		//After Joined Thread exits
		//Current Thread grabs the retval
		if(retval)
			*retval = thread_c->retval;
	}

	//If thread_z is a zombie then destroy it
	if(thread_z->state == ZOMBIE){
		uthread_ctx_destroy_stack(thread_n->stack);
		free(thread_n);
	}

	preempt_enable();
	return 0;
}
