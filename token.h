/*
FIFO token_queue, adapted from OSTEP, section 29.3
Concurrent token_queue
*/

#ifndef TOKEN_H
#define TOKEN_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>

#include "common.h"




struct token_node_t{
    int index;
    struct token_node_t *next;
};


typedef struct {
    struct token_node_t *head;
    struct token_node_t *tail;
    sem_t head_lock;
    sem_t tail_lock;
} token_queue_t;

typedef struct{
    sem_t tokens_available;
    token_queue_t tq;
    sem_t sem_arr[NO_OF_TOKENS];
    int ct_assns[NO_OF_TOKENS];//mapping from index to client id
} main_token_system_t;

void token_queue_init(token_queue_t *q);
void token_queue_cleanup(token_queue_t *q);
void token_queue_push(token_queue_t *q, int i);
int token_queue_pop(token_queue_t *q);

//If token available, returns nonnegative number, else returns -1
//client doesnt have to wait
void mt_token_system_init(main_token_system_t *mt);
int mt_token_get(main_token_system_t *mt, int u_client_id);
int mt_token_tryget(main_token_system_t *mt, int u_client_id);
int mt_token_wait_for_signal(main_token_system_t *mt, int index, int u_client_id);
int mt_token_give_signal(main_token_system_t *mt, int index);
int mt_token_return(main_token_system_t *mt, int index, int u_client_id);
void mt_token_system_cleanup(main_token_system_t *mt);


//usage
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
/*
int main(){
    int i;
    pthread_t pd[CLIENTS];
    mt_token_system_init(&mt);
    for(i = 0; i < CLIENTS; ++i){
        pthread_create(pd+i, 0, client_enter_restaurant, (void*) i);
    }
    for(i = 0; i < CLIENTS; ++i){
        pthread_join(pd[i], NULL);
    }
    mt_token_system_cleanup(&mt);
    return 0;
}
*/
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//FUNCTIONS FOR MAIN TOKEN SYSTEM
void mt_token_system_init(main_token_system_t *mt){
    int i;
    sem_init(&mt->tokens_available, 1, NO_OF_TOKENS);
    memset(&(mt->ct_assns), -1, NO_OF_TOKENS * sizeof(int));
    token_queue_init(&mt->tq);
    for(i = 0; i < NO_OF_TOKENS; ++i){
        token_queue_push(&mt->tq, i);
        sem_init(&(mt->sem_arr[i]), 1, 1);
    }
}

void mt_token_system_cleanup(main_token_system_t *mt){
    int i;
    sem_destroy(&(mt->tokens_available));
    token_queue_cleanup(&mt->tq);
    for(i = 0; i < NO_OF_TOKENS; ++i){
        sem_destroy(&(mt->sem_arr[i]));
    }
}


/*
If token available, returns nonnegative number, else
returns -1
*/
int mt_token_get(main_token_system_t *mt, int u_client_id){
    sem_wait(&(mt->tokens_available));
    int index =  token_queue_pop(&(mt->tq));
    if(index != -1){
        mt->ct_assns[index] = u_client_id;
    }
    return index;
}

int mt_token_tryget(main_token_system_t *mt, int u_client_id){
    int index;
    if (sem_trywait(&(mt->tokens_available)) == FAILED) return FAILED;
    index =  token_queue_pop(&(mt->tq));
    if(index != -1){
        mt->ct_assns[index] = u_client_id;
    }
    return index;
}

int mt_token_wait_for_signal(main_token_system_t *mt, int index, int u_client_id){
    if(u_client_id != (mt->ct_assns[index])) return -1;
    sem_wait(&(mt->sem_arr[index]));
    return 0;
}

int mt_token_give_signal(main_token_system_t *mt, int index){
    sem_post(&(mt->sem_arr[index]));
    return 0;
}

int mt_token_return(main_token_system_t *mt, int index, int u_client_id){
    if(u_client_id != (mt->ct_assns[index])) return -1;
    mt->ct_assns[index] = -1;
    token_queue_push(&(mt->tq), index);
    sem_post(&(mt->tokens_available));
    mt_token_give_signal(mt, index);
    return 0;
}

//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//FUNCTIONS FOR TOKEN QUEUE
void token_queue_init(token_queue_t *q) {
    struct token_node_t *dummy_node_p = malloc(sizeof(struct token_node_t));
    dummy_node_p->index = 666;
    dummy_node_p->next = NULL;
    q->head = q->tail = dummy_node_p;
    sem_init(&(q->head_lock), 1, 1);
    sem_init(&(q->tail_lock), 1, 1);
}


void token_queue_cleanup(token_queue_t *q){
    free (q->head);
    sem_destroy(&q->head_lock);
    sem_destroy(&q->tail_lock);
}

/*
Pushes new indexes to end of token_queue
*/
void token_queue_push(token_queue_t *q, int i) {
    struct token_node_t *tmp = malloc(sizeof(struct token_node_t));
    assert(tmp != NULL);
    tmp->index = i;
    tmp->next = NULL;

    sem_wait(&q->tail_lock);
    q->tail->next = tmp;
    q->tail = tmp;
    sem_post(&q->tail_lock);
}

/*
Pops indices from beginning of token_queue
*/
int token_queue_pop(token_queue_t *q) {
    int v;

    struct token_node_t *tmp;
    struct token_node_t *newHead;
    sem_wait(&q->head_lock);
    tmp = q->head;
    newHead = tmp->next;
    if (newHead == NULL) {
        v = -1; // token_queue was empty
    }else{
        v = newHead->index;
        q->head = newHead;
    }
    sem_post(&q->head_lock);
    free(tmp);
    return v;
}
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#endif













































































//end
