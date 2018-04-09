/*
FIFO Queue, adapted from OSTEP, section 29.3
Concurrent queue
*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
/*
typedef struct{
    int a;
}value_t;
*/
typedef int value_t;

typedef struct _node_t{
    value_t value;
    struct _node_t *next;
}node_t;

typedef struct {
    node_t *head;
    node_t *tail;
    pthread_mutex_t head_lock;
    pthread_mutex_t tail_lock;
} queue_t;

void queue_init(queue_t *q) {
    node_t *dummy_node = malloc(sizeof(node_t));
    dummy_node->next = NULL;
    q->head = q->tail = dummy_node;
    pthread_mutex_init(&q->head_lock, NULL);
    pthread_mutex_init(&q->tail_lock, NULL);
}


/*
Pushes new values to end of queue
*/
void push_to_queue(queue_t *q, value_t v) {
    node_t *tmp = malloc(sizeof(node_t));
    assert(tmp != NULL);
    tmp->value = v;
    tmp->next = NULL;

    pthread_mutex_lock(&q->tail_lock);
    q->tail->next = tmp;
    q->tail = tmp;
    pthread_mutex_unlock(&q->tail_lock);
}

/*
Pops values from beginning of queue
returns pointer to value_t it is responsibility of caller
to deallocate memeory once done with value
if queue empty, returns -1, else returns zero if able
to return value
*/
int pop_from_queue(queue_t *q, value_t *v_p) {
    pthread_mutex_lock(&q->head_lock);
    node_t *tmp = q->head;
    node_t *newHead = tmp->next;
    if (newHead == NULL) {
        pthread_mutex_unlock(&q->head_lock);
        return -1; // queue was empty
    }
    *v_p = newHead->value;
    q->head = newHead;
    pthread_mutex_unlock(&q->head_lock);
    free(tmp);
    return 0;
}


int main(){
    queue_t q;
    queue_init(&q);
    push_to_queue(&q, 1);
    push_to_queue(&q, 2);
    push_to_queue(&q, 3);
    push_to_queue(&q, 4);
    value_t x;
    pop_from_queue(&q, &x);
    printf("%d\n", x);
    pop_from_queue(&q, &x);
    printf("%d\n", x);
    pop_from_queue(&q, &x);
    printf("%d\n", x);
    pop_from_queue(&q, &x);
    printf("%d\n", x);
    pop_from_queue(&q, &x);
    printf("%d\n", x);
    return 0;
}



























































//end
