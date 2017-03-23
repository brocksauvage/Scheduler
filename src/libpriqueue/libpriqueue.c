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
	q->back = NULL;
}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{
	p_node_t *node = malloc (sizeof(ptr));
	memset(node, 0, sizeof(ptr));
	node = ptr;

	if(priqueue_size(q) == 0)
	{
		q->front = node;
		q->back = node;
		q->size++;
		return 0;
	}
	else if(priqueue_size(q) == 1)
	{

	}

	return -1;
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
		return q->front;
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
	if(priqueue_size(q) == 0)
	{
		return NULL;
	}
	else if(priqueue_size(q) == 1)
	{
		free(&q->front);
		q->front = NULL;
		q->back = NULL;
		q->size--;
		return(NULL);
	}
	else
	{
		p_node_t *temp = q->front->prev;
		temp->next = NULL;
		free(&q->front);
		q->size--;
		return (&q->front);
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

	if(priqueue_size(q) == 1 || index == 0)
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
	return 0;
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
		q->back = NULL;
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
  		if(temp->next->next == NULL)
  		{
  			q->back = temp;
  			free(temp->next);
        temp->next = NULL;
  			q->size--;
  		}
      else
      {
        p_node_t *delete = temp->next;
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
	while(&q->front != NULL)
	{
		priqueue_poll(&q);
	}
	free(&q);
}
