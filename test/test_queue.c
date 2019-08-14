#include <queue.h>
#include <assert.h>
#include <stdio.h>


/*Tester for creating a queue*/
void test_create(void)
{
	queue_t q = queue_create();
	
	printf("test starting: test_create\n");
	assert(q != NULL);
	printf("test completed: test_create\n\n");
}

/*Tester for enqueueing and dequeuing one data*/
void test_queue_simple(void)
{
	queue_t q = queue_create();
	int data = 3, *ptr;

	printf("test starting: test_queue_simple\n");
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	assert(ptr == &data);
	printf("test completed: test_queue_simple\n\n");
}

/*Tester for enqueueing and dequeuing multiple data*/
void test_queue_multiple(void)
{
	queue_t q = queue_create();
	int data[] = {1, 2, 3, 4, 5};
	int *ptr;

	printf("test starting: test_queue_multiple\n");
	queue_enqueue(q, &data[0]);
	queue_enqueue(q, &data[1]);
	queue_enqueue(q, &data[2]);
	queue_enqueue(q, &data[3]);
	queue_enqueue(q, &data[4]);

	queue_dequeue(q, (void**)&ptr);
	assert(ptr == &data[0]);

	queue_dequeue(q, (void**)&ptr);
	assert(ptr == &data[1]);

	queue_dequeue(q, (void**)&ptr);
	assert(ptr == &data[2]);

	queue_dequeue(q, (void**)&ptr);
	assert(ptr == &data[3]);

	queue_dequeue(q, (void**)&ptr);
	assert(ptr == &data[4]);
	printf("test completed: test_queue_multiple\n\n");
}

/*Tester for checking queue size*/
void test_queue_size(void)
{
	int data[] = {1, 2, 3, 4, 5};
	int *ptr;
	int size;
	queue_t q = queue_create();

	printf("test starting: test_queue_size\n");
	size = queue_length(q);
	assert(size == 0);
	queue_enqueue(q, &data[0]);
	queue_enqueue(q, &data[1]);
	queue_enqueue(q, &data[2]);
	queue_enqueue(q, &data[3]);
	queue_enqueue(q, &data[4]);

	size = queue_length(q);
	assert(size == 5);

	queue_dequeue(q, (void**)&ptr);
	assert(ptr == &data[0]);

	queue_dequeue(q, (void**)&ptr);
	assert(ptr == &data[1]);

	queue_dequeue(q, (void**)&ptr);
	assert(ptr == &data[2]);

	size = queue_length(q);
	assert(size == 2);

	printf("test completed: test_queue_size\n\n");
}

/* Callback function that increments items by a certain value */
static int inc_item(void *data, void *arg)
{
	int *a = (int*)data;
	int inc = (int)(long)arg;

	*a += inc;

	return 0;
}

/* Callback function that finds a certain item according to its value */
static int find_item(void *data, void *arg)
{
	int *a = (int*)data;
	int match = (int)(long)arg;

	if (*a == match)
		return 1;

	return 0;
}

/*Tester for iterator function*/
void test_iterator(void)
{
	printf("test starting: test_queue_size\n");
	queue_t q;
	int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int i;
	int *ptr;

	/* Initialize the queue and enqueue items */
	q = queue_create();
	for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
		queue_enqueue(q, &data[i]);

	/* Add value '1' to every item of the queue */
	queue_iterate(q, inc_item, (void*)1, NULL);
	assert(data[0] == 2);

	/* Find and get the item which is equal to value '5' */
	ptr = NULL;
	queue_iterate(q, find_item, (void*)5, (void**)&ptr);
	assert(ptr != NULL);
	assert(*ptr == 5);
	assert(ptr == &data[3]);
	printf("test completed: test_queue_size\n\n");
}

/*Tester for deleting data*/
void test_queue_delete(void)
{
	int data[] = {1, 2, 3, 4, 5};
	int size, retval;
	queue_t q = queue_create();

	printf("test starting: test_queue_delete\n");
	queue_enqueue(q, &data[0]);
	queue_enqueue(q, &data[1]);
	queue_enqueue(q, &data[2]);
	queue_enqueue(q, &data[3]);
	queue_enqueue(q, &data[4]);

	retval = queue_delete(q, &data[2]);
	size = queue_length(q); 
	assert(size == 4);
	assert(retval == 0);
	
	retval = queue_delete(q, &data[0]);
	size = queue_length(q); 
	assert(size == 3);
	assert(retval == 0);
	
	retval = queue_delete(q, &data[0]);
	size = queue_length(q); 
	assert(size == 3);
	assert(retval == -1);
	
	retval = queue_delete(q, &data[4]);
	size = queue_length(q); 
	assert(size == 2);
	assert(retval == 0);

	retval = queue_delete(q, &data[3]);
	size = queue_length(q); 
	assert(size == 1);
	assert(retval == 0);
	printf("test completed: test_queue_delete\n\n");
}

/*Tester for enqueueing and dequeuing chars*/
void test_queue_chars(void)
{
	char data[] = {'a', 'b', 'c', 'd', 'e'};
	char *ptr;
	queue_t q = queue_create();


	printf("test starting: test_queue_chars\n");
	queue_enqueue(q, &data[0]);
	queue_enqueue(q, &data[1]);
	queue_enqueue(q, &data[2]);
	queue_enqueue(q, &data[3]);
	queue_enqueue(q, &data[4]);

	queue_dequeue(q, (void**)&ptr);
	assert(ptr == &data[0]);

	queue_dequeue(q, (void**)&ptr);
	assert(ptr == &data[1]);

	queue_dequeue(q, (void**)&ptr);
	assert(ptr == &data[2]);

	queue_dequeue(q, (void**)&ptr);
	assert(ptr == &data[3]);

	queue_dequeue(q, (void**)&ptr);
	assert(ptr == &data[4]);
	printf("test completed: test_queue_chars\n\n");
}

/*Tester for enqueueing and dequeuing multiple data types*/
void test_queue_multiple_type(void)
{
	char char_data = 'c';
	char *char_ptr;
	int int_data = 1;
	int *int_ptr;
	float float_data = 2.0;
	float *float_ptr;
	double double_data = 2.01;
	double *double_ptr;
	queue_t q = queue_create();

	printf("test starting: test_queue_multpile_types\n");
	queue_enqueue(q, &char_data);
	queue_enqueue(q, &int_data);
	queue_enqueue(q, &float_data);
	queue_enqueue(q, &double_data);

	queue_dequeue(q, (void**)&char_ptr);
	assert(char_ptr == &char_data);

	queue_dequeue(q, (void**)&int_ptr);
	assert(int_ptr == &int_data);

	queue_dequeue(q, (void**)&float_ptr);
	assert(float_ptr == &float_data);

	queue_dequeue(q, (void**)&double_ptr);
	assert(double_ptr == &double_data);
	printf("test completed: test_queue_multiple_types\n\n");
}
int main(int argc, char** argv){
	test_create();
	test_queue_simple();
	test_queue_multiple();
	test_queue_size();
	test_queue_delete();
	test_queue_chars();
	test_iterator();
	test_queue_multiple_type();

	return 0;
}
