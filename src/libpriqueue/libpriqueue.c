/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"


/**
  Initializes the priqueue_t data structure.

  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
	q->size = 0;
	q->front = NULL;
	q->comparer = comparer;
}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{
	p_node_t *node = malloc (sizeof(p_node_t));
	node->job = ptr;
	node->next = NULL;

	int val = *((int *) node -> job);
	printf("%d", val);
	//node->prev = NULL;
	if(priqueue_size(q) == 0)
	{
		q->front = node;
		q->back = node;
		q->size++;
		return 0;
	}
	else if(priqueue_size(q) == 1)
	{
		if(q->comparer(node->job, q->front) < 0)
		{
			node->next = q->front;
			q->front = node;
			q->back = node->next;
		}
		else
		{
				q->front->next = node;
				q->back = node;
		}
			q->size++;
			return 1;
	}
	else
	{
		int index = 0;
		p_node_t *temp = q->front;
		p_node_t *temp2 =q->front;
		while(temp->next != NULL)
		{
			temp2 = temp;		
			//printf("%d", temp2->job);
			if(q->comparer(node, temp) < 0)
			{

				printf("%s", "inside ");
				break;
			}	
			temp = temp->next;
			index++;
			printf("%s", "here ");

		}

		if(temp->next == NULL)
		{
			temp->next = node;
			q->size++;
			printf("%s", "outside1\n");
		}
		else
		{
			node->next = temp;
			temp2->next = node;
			q->size++;
			printf("%s", "outside2\n");
		}
		printf("%s", "finished\n");
		return index;
	}
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{
	if(priqueue_size(q) == 0)
	{
		return NULL;
	}
	else
	{
		return (q->front);
	}
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{
	p_node_t *temp = q->front;
	while(temp->next != NULL)
	{
		int val = *((int *) temp -> job);
		printf("%d", val);
		temp = temp->next;
	}	
	printf("%s", "poll\n");
	if(priqueue_size(q) == 0)
	{
		return NULL;
	}
	else
	{
		p_node_t *temp = q->front;
		q->front = q->front->next;
		free(temp);
		q->size--;
		return (q->front->job);
	}
}


/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
 */
void *priqueue_at(priqueue_t *q, int index)
{
  if(index > priqueue_size(q) || index < 0)
	{
		return NULL;
	}

	if(index == 0)
	{
	   return(q->front);
	}
	else
	{
		p_node_t *temp = q->front;

  	while(index > 0)
  	{
  		temp = temp->next;
  		index--;
  	}
    return(temp);

	}
	return 0;
}


/**
  Removes all instances of ptr from the queue.

  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
int priqueue_remove(priqueue_t *q, void *ptr)
{
	printf("In remove");
	
	/*printf("%s", "remove\n");
	if(priqueue_size(q) == 0)
	{
		return 0;
	}
	else if(priqueue_size(q) == 1)
	{
		priqueue_poll(q);
		return (1);
	}
	else
	{
		int removals = 0;
		p_node_t *temp = q->front;
		p_node_t *temp2;
		while(temp->next != NULL)
		{
			temp2 = temp;
			//printf("while");
			if(temp->job == ptr)
			{
				printf("job pointer");
				if(temp == q->front)
				{
					temp = priqueue_poll(q);
					printf("first branch");
					removals++;
				}
				else if(temp->next == NULL)
				{
					printf("%s", "else if\n");
					temp2->next = NULL;
					free(temp);
					q->size--;
					removals++;
					return removals;
				}
				else
				{
								printf("%s", "else\n");
					temp2 -> next = temp->next;
					free(temp);
					q->size--;
					removals++;
				}
			}
			else
			{
				temp = temp->next;
			}

		}
		return (removals);
	}*/
}


/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{
	if(index > priqueue_size(q) || index < 0)
	{
		return NULL;
	}

	if(priqueue_size(q) == 1)
	{
		free(&q->front);
		q->front = NULL;
		q->size--;
	}
	else
	{
		p_node_t *temp = q->front;
    if(index == 0)
    {
      q->front = temp->next;
      free(temp);
      q->size--;
    }
    else
    {
  		while(index > 1)
  		{
  			temp = temp->next;
  			index--;
  		}
			p_node_t *delete = temp->next;
  		if(delete->next == NULL)
  		{
  			q->back = temp;
  			free(delete);
        temp->next = NULL;
  			q->size--;
  		}
      else
      {
        temp->next = delete->next;
        free(delete);
        q->size--;
      }
    }

	}
	return 0;
}


/**
  Returns the number of elements in the queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
	return q->size;
}


/**
  Destroys and frees all the memory associated with q.

  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
	while(q->front != NULL)
	{
		priqueue_poll(q);
	}
	free(q);
}
