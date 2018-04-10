#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "counter.h"
#include "common.h"




typedef struct{
    int buffer[BQ_MAX];
    int fill;
    int use;
    sem_t empty;
    sem_t full;
    sem_t lock;
}bounded_queue_t;

bounded_queue_t b;
counter_t no_served;

void bounded_queue_init(bounded_queue_t *bq){
    bq->fill = 0;
    bq->use = 0;
    sem_init(&bq->empty, 1, BQ_MAX);
    sem_init(&bq->full, 1, 0);
    sem_init(&bq->lock, 1, 1);
}

void bounded_queue_cleanup(bounded_queue_t *bq){
    sem_destroy(&bq->empty);
    sem_destroy(&bq->full);
    sem_destroy(&bq->lock);
}


void bounded_queue_push(bounded_queue_t *bq, int value){
    sem_wait(&bq->empty);
    sem_wait(&bq->lock);
    bq->buffer[bq->fill] = value;
    bq->fill = (bq->fill + 1)%BQ_MAX;
    sem_post(&bq->lock);
    sem_post(&bq->full);
}

int bounded_queue_trypush(bounded_queue_t *bq, int value){
    if(sem_trywait(&bq->empty) == FAILED) return FAILED;
    sem_wait(&bq->lock);
    bq->buffer[bq->fill] = value;
    bq->fill = (bq->fill + 1)%BQ_MAX;
    sem_post(&bq->lock);
    sem_post(&bq->full);

    return SUCCESSFUL;
}

//return 0 if pop successful, -1 if nothing to pop
void bounded_queue_pop(bounded_queue_t *bq, int* v_p){
    sem_wait(&bq->full);
    sem_wait(&bq->lock);
    *v_p = bq->buffer[bq->use];
    bq->use = (bq->use + 1) % BQ_MAX;
    sem_post(&bq->lock);
    sem_post(&bq->empty);
}

//return 0 if pop successful, -1 if nothing to pop
int bounded_queue_trypop(bounded_queue_t *bq, int* v_p){
    if(sem_trywait(&bq->full) == FAILED) return FAILED;
    sem_wait(&bq->lock);
    *v_p = bq->buffer[bq->use];
    bq->use = (bq->use + 1) % BQ_MAX;
    sem_post(&bq->lock);
    sem_post(&bq->empty);

    return SUCCESSFUL;
}



void* cashier_serve_client(void *arg){
    int temp, status;
    time_t rawtime;
    struct tm * timeinfo;

    while((counter_get_c(&no_served)) != CLIENTS){
        status = bounded_queue_trypop(&b, &temp);
        if(status == SUCCESSFUL){
            counter_increment(&no_served);
            time ( &rawtime );
            timeinfo = localtime ( &rawtime );
            printf("\t~Client: %d served by cashier: %d at time: %s\t~N served so far: %d\n",
                    temp, (int)arg, asctime(timeinfo), counter_get_c(&no_served));
            fflush(stdout);
        }else if(status == FAILED) continue;
        //sleep(2);
    }
    return NULL;
}

void* client_enter_queue(void *arg){
    time_t rawtime;
    struct tm * timeinfo;
    int waiting_time;
    while(1){
        if((bounded_queue_trypush(&b, (int)arg)) == FAILED){
            waiting_time = (rand() % BQ_MAX_WAITING_TIME) + 1;
            printf("Client: %d queue full, trying again after %d secs\n", (int)arg, waiting_time);
            sleep(waiting_time);
        }else{
            time ( &rawtime );
            timeinfo = localtime ( &rawtime );
            printf("Client: %d got into queue at time %s", (int) arg,
                    asctime(timeinfo));
            break;
        }
    }
    return NULL;
}

int main(){
    int i;
    pthread_t pd[CLIENTS];
    pthread_t cm[CASHIERS];

    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf ( "Current local time and date: %s\n", asctime (timeinfo) );

    bounded_queue_init(&b);
    counter_init(&no_served);
    for(i = 0; i < CLIENTS; ++i){
        pthread_create(pd+i, 0, client_enter_queue, (void*) i);
    }

    for(i = 0; i < CASHIERS; ++i){
        pthread_create(cm+i, 0, cashier_serve_client, (void*)i);
    }

    for(i = 0; i < CLIENTS; ++i){
        pthread_join(pd[i], NULL);
    }

    for(i = 0; i < CASHIERS; ++i){
        pthread_join(cm[i], NULL);
    }

    bounded_queue_cleanup(&b);
    counter_cleanup(&no_served);
    return 0;
}





//end
