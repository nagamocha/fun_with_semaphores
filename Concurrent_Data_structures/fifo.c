/*
FIFO Queue, adapted from OSTEP, section 29.3
Concurrent queue
*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <semaphore.h>


typedef int value_t;

typedef struct _node_t{
    value_t value;
    struct _node_t *next;
}node_t;

typedef struct {
    node_t *head;
    node_t *tail;
    sem_t head_lock;
    sem_t tail_lock;
} queue_t;

void queue_init(queue_t *q) {
    node_t *dummy_node = malloc(sizeof(node_t));
    dummy_node->next = NULL;
    q->head = q->tail = dummy_node;
    sem_init(&(q->head_lock), 1, 1);
    sem_init(&(q->tail_lock), 1, 1);
}


void queue_cleanup(queue_t *q){
    free (q->head);
    sem_destroy(&q->head_lock);
    sem_destroy(&q->tail_lock);
}

/*
Pushes new values to end of queue
*/
void queue_push(queue_t *q, value_t v) {
    node_t *tmp = malloc(sizeof(node_t));
    assert(tmp != NULL);
    tmp->value = v;
    tmp->next = NULL;

    sem_wait(&q->tail_lock);
    q->tail->next = tmp;
    q->tail = tmp;
    sem_post(&q->tail_lock);
}

/*
Pops values from beginning of queue
returns pointer to value_t it is responsibility of caller
to deallocate memeory once done with value
if queue empty, returns -1, else returns zero if able
to return value
*/
int queue_pop(queue_t *q, value_t *v_p) {
    sem_wait(&q->head_lock);
    node_t *tmp = q->head;
    node_t *newHead = tmp->next;
    if (newHead == NULL) {
        sem_post(&q->head_lock);
        return -1; // queue was empty
    }
    *v_p = newHead->value;
    q->head = newHead;
    sem_post(&q->head_lock);
    free(tmp);
    return 0;
}


int main(){
    queue_t q;
    queue_init(&q);
    queue_push(&q, 1);
    queue_push(&q, 2);
    queue_push(&q, 3);
    queue_push(&q, 4);
    value_t x;
    queue_pop(&q, &x);
    printf("%d\n", x);
    queue_pop(&q, &x);
    printf("%d\n", x);
    queue_pop(&q, &x);
    printf("%d\n", x);
    queue_pop(&q, &x);
    printf("%d\n", x);
    queue_pop(&q, &x);
    printf("%d\n", x);
    queue_cleanup(&q);
    return 0;
}



























































//end
