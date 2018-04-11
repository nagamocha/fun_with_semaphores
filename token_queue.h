#ifndef TOKEN_QUEUE_H
#define TOKEN_QUEUE_H

#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "common.h"

typedef struct{
    int buffer[NO_OF_TOKENS];
    int fill;
    int use;
    sem_t empty;
    sem_t full;
    sem_t lock;
}token_queue_t;


void token_queue_init(token_queue_t *tq){
    tq->fill = 0;
    tq->use = 0;
    sem_init(&tq->empty, 1, NO_OF_TOKENS);
    sem_init(&tq->full, 1, 0);
    sem_init(&tq->lock, 1, 1);
}

void token_queue_cleanup(token_queue_t *tq){
    sem_destroy(&tq->empty);
    sem_destroy(&tq->full);
    sem_destroy(&tq->lock);
}


void token_queue_push(token_queue_t *tq, int value){
    sem_wait(&tq->empty);
    sem_wait(&tq->lock);
    tq->buffer[tq->fill] = value;
    tq->fill = (tq->fill + 1)%NO_OF_TOKENS;
    sem_post(&tq->lock);
    sem_post(&tq->full);
}

int token_queue_trypush(token_queue_t *tq, int value){
    if(sem_trywait(&tq->empty) == FAILED) return FAILED;
    sem_wait(&tq->lock);
    tq->buffer[tq->fill] = value;
    tq->fill = (tq->fill + 1)%NO_OF_TOKENS;
    sem_post(&tq->lock);
    sem_post(&tq->full);

    return SUCCESSFUL;
}

//return 0 if pop successful, -1 if nothing to pop
int token_queue_pop(token_queue_t *tq){
    int v = -1;
    sem_wait(&tq->full);
    sem_wait(&tq->lock);
    v = tq->buffer[tq->use];
    tq->use = (tq->use + 1) % NO_OF_TOKENS;
    sem_post(&tq->lock);
    sem_post(&tq->empty);
    return v;
}

//return 0 if pop successful, -1 if nothing to pop
int token_queue_trypop(token_queue_t *tq){
    int v = -1;
    if(sem_trywait(&tq->full) == FAILED) return -1;
    sem_wait(&tq->lock);
    v = tq->buffer[tq->use];
    tq->use = (tq->use + 1) % NO_OF_TOKENS;
    sem_post(&tq->lock);
    sem_post(&tq->empty);

    return v;
}



#endif


//end
