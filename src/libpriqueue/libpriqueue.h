/** @file libpriqueue.h
 */

#ifndef LIBPRIQUEUE_H_
#define LIBPRIQUEUE_H_

typedef struct p_node_t p_node_t;
/**
	Process Nodes
*/
struct p_node_t
{
	void *job;
	p_node_t *next;
	//p_node_t *prev;
};

/**
  Priqueue Data Structure
*/
typedef struct _priqueue_t
{
	int size;
	p_node_t *front;
	int (*comparer) (const void*, const void *);
	p_node_t *back;
} priqueue_t;



void   priqueue_init     (priqueue_t *q, int(*comparer)(const void *, const void *));

int    priqueue_offer    (priqueue_t *q, void *ptr);
void * priqueue_peek     (priqueue_t *q);
void * priqueue_poll     (priqueue_t *q);
void * priqueue_at       (priqueue_t *q, int index);
int    priqueue_remove   (priqueue_t *q, void *ptr);
void * priqueue_remove_at(priqueue_t *q, int index);
int    priqueue_size     (priqueue_t *q);

void   priqueue_destroy  (priqueue_t *q);

#endif /* LIBPQUEUE_H_ */
