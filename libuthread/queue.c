#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

typedef struct node* node_t;

struct queue {
	struct node *head;
	struct node *tail;
	int size;	
};

struct node {
	struct node *next;
	struct node *prev;
	void *data;
};


queue_t queue_create(void)
{
	queue_t cue;
	cue = (queue_t)malloc(sizeof(queue_t));
	
	if(cue != NULL){
		cue->size = 0;
		cue->head = NULL;
		cue->tail = NULL;
	}
	
	return cue;
}

int queue_destroy(queue_t queue)
{
	if(queue == NULL || queue->size != 0)
		return -1;

	free(queue);
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	node_t temp;

	if(queue == NULL || data == NULL)
		return -1;

	(queue->size)++;
	temp = (node_t) malloc(sizeof(node_t));

	if(temp == NULL)
		return -1;

	temp->next = NULL;
	temp->prev = queue->tail;
	temp->data = data;

	if(queue->size == 1)//If only one item put to head
		queue->head = temp;

	else if(queue->size == 2)//If 2 items put to the next node of head
		queue->head->next = temp;

	else //Else put it in the next node of the tail
		queue->tail->next = temp;

	queue->tail = temp;

	if(queue == NULL)
		return -1;

	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if(queue == NULL || data == NULL || queue->size == 0)
		return -1;

	//Dequeues data from the head since FIFO
	*data = queue->head->data;
	(queue->size)--;

	if(queue->size == 0)
		free(queue->head);
	
	else{
		queue->head = queue->head->next;
		free(queue->head->prev);
	}

	return 0;
}

int queue_delete(queue_t queue, void *data)
{

	int i;
	node_t temp = queue->head;

	if(queue == NULL || data == NULL)
		return -1;

	//Tries to find the data
	for(i = 0; i < queue->size; i++){
		
		if(temp->data != data)
			temp = temp->next;

		else{
			(queue->size)--;

			//If at the front then put next node to head
			if(temp == queue->head)
				queue->head = queue->head->next;

			//else if tail put to previous
			else if(temp == queue->tail)
				queue->tail = queue->tail->prev;
				
			else{
				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;
			}

			free(temp);
			return 0;
		}
	}

	return -1;
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
	node_t temp = queue->head;
	int i, flag;

	if(queue == NULL || func == NULL)
		return -1;

	//Goes through queue until function succeeds
	for(i = 0; i < queue->size; i++){
		flag = func(temp->data, arg);

		if(flag == 1){
			if(data != NULL){
				*data = temp->data;
				data = &(temp->data);
			}

			break;
		}

		temp = temp->next;	
	}

	return 0;
}

int queue_length(queue_t queue)
{
	if(queue == NULL)
		return -1;

	return queue->size;
}
