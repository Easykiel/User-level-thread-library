#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "preempt.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

/*Signal Handler that just calls yield*/
void sig_inthandler(int sig_num)
{
	uthread_yield();
}

/*Disables Preempt*/
void preempt_disable(void)
{
	sigprocmask(SIG_BLOCK, (sigset_t*)SIGVTALRM, NULL);
}

/*Enables Preempt*/
void preempt_enable(void)
{
	sigprocmask(SIG_UNBLOCK, (sigset_t*)SIGVTALRM, NULL);
}

/*Starts Preempt*/
void preempt_start(void)
{
	struct itimerval new;

	signal(SIGVTALRM, sig_inthandler);
	new.it_value.tv_sec = HZ / 1000;
	new.it_value.tv_usec = HZ * 1000 % 1000000;
	new.it_interval = new.it_value;
	setitimer(ITIMER_VIRTUAL, &new, NULL);	
}

