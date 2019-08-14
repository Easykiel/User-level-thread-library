#Introduction

In this project, we were tasked with developing threads by developing
the containers and implementation along with testing the program ourselves to
make sure that it works. We started by creating the queue that would be used in
the thread application, then designing the thread, then adding in preempt
functionality.

#Implementation
##Queue

To create the queue, we started with the data structure that would be
used to hold the queue data. We figured that the best data structure to be used
in the queue would be a double linked list, since we can enqueue and dequeue in
FIFO order in O(1) time by keeping a pointer for the head and tail, which we
kept in the main struct along with a variable for the queue size. For dequeue
and enqueue, it was easy for us to implement since it was just a matter of
moving the head and tail pointers, and freeing or mallocing the respective
nodes. For queue delete, we would iterate through the queue from the front since
the front would contain the oldest data and would stop if the correct node was
found. We then move the prev and next node pointers from the next and previous
nodes respectively(if the node isn't the head or tail) so that they would skip
over the node being deleted. For queue iteration, we would iterate through the
queue and apply the function to each node's data. Lastly, for queue size, we
would simply return the data for size in the queue struct.

##Uthread

To create the threads, we first started by making the data structure to
contain information about the thread. We added a integer variable that would
keep track of the thread state, a flag to see if the thread was joined, a return
value for the joined thread, a TID to identify the thread, a ctx variable for
context switching, a stack pointer, and another TID for a joined thread. We then
added 3 queues to keep track of thread states: a ready queue for available
threads, a running queue for active threads, and a blocked queue for dead and
blocked threads. We first started with thread creation. If the queues are NULL,
then we know that we are creating the first thread, so we first create the main
thread along with all the queues, and add the main thread to the running queue
since it should be the only one running. For every subsequent thread, we
initialize the thread struct with defaults and thread information and then throw
it in the ready queue. For thread joining, we first check if thread can be
joined, then proceed to find the running thread and the state of the thread to
be joined. If the thread can be joined, we block the current thread and store it
in the blocked queue, and then set up the next ready thread to start running
while saving the information about the parent thread for when the thread exits
and the parent needs to be unblocked. If the thread is already dead, then the
parent will just store the information. For thread yield, we need to switch out
the running thread with the next available thread. So, we dequeue once from the
running and the ready queues to get those threads, swap the states for both,
then enqueue them in their respective queues and context switch. For uthread
exit, we start by moving the current running thread to the zombie state since it
should be considered dead, then move it to the blocked queue. Here, we also need
to check if the thread was joined, because in that case we have to find the
blocked thread in the blocked queue and enqueue it back into the ready queue.
Once that's done we set the next ready thread to running. 

##Preemption

Lastly for preemption, we started by writing the enable and disable,
since we just needed to use sigprocmask to unblock or block our sigvtalrm
signal. Preempt start was more tricky but we ended up using the signal function
to set up our interrupt handler, which would call uthread yield. We set it up
like this beause the interrupt handler will trigger everytime the timer goes off
and we want preemption to swap contexts at that time so that the thread can
change. For our timer, we set it up so that the clock will trigger at 100Hz and
we used setitimer to set this. To set up preemption, we use the start function
in the function that creates the main thread, since this is where we can
start utilizing preemption. We then put disables at the beginning of every
thread function and enables at the end of every thread function since we didn't
want preemption to go off when we were changing the thread information and cause
data corruption.

#Testing

To test our queue, threads, and preemption, we made several test cases
trying to think of many different possibilities, and seeing if our output was as
expected. For the queue, we tested the basics by calling different queue
functions and seeing if they worked. We then mixed functions together in our
test cases and observed if the output was correct. For the threads, we used the
test functions that were provided by the program document to test our basic
functionality. We then tried using test cases with multiple joins to see if our
program would work as expected, and collect the zombie thread information
correctly. For preemption, we decided to test it by using the thread yield test
and cut out all the yields and add a while(1) loop in the first thread. This
would help us test whether or not the timer interrupts were triggering
correctly since we can see if all the threads would execute without yields. We
also added the while(1) loop since we figured that preemption was implemented to
avoid infinite loops breaking the thread.

#Conclusion

In the end, we were able to implement threading into our programs and
learn the concepts behind threading. One of the problems we encountered with
this program was with threading. We tried implemting the threading with one big
queue, but we realized that it would take too long to get to ready threads when
there are a lot of blocked or zombie threads in the queue and it defeated the
purpose of using a queue for threading since we would end up just iterating
through the thread to find the correct thread.
