#ifndef LINEUP_QUEUE_H
#define LINEUP_QUEUE_H


#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "counter.h"
#include "common.h"


typedef struct{
    order_t buffer[LINEUP_MAX];
    int fill;
    int use;
    sem_t empty;
    sem_t full;
    sem_t lock;
}lineup_queue_t;


void lineup_queue_init(lineup_queue_t *lq){
    lq->fill = 0;
    lq->use = 0;
    sem_init(&lq->empty, 1, LINEUP_MAX);
    sem_init(&lq->full, 1, 0);
    sem_init(&lq->lock, 1, 1);
}

void lineup_queue_cleanup(lineup_queue_t *lq){
    sem_destroy(&lq->empty);
    sem_destroy(&lq->full);
    sem_destroy(&lq->lock);
}


void lineup_queue_push(lineup_queue_t *lq, order_t client_order){
    sem_wait(&lq->empty);
    sem_wait(&lq->lock);
    lq->buffer[lq->fill] = client_order;
    lq->fill = (lq->fill + 1)%LINEUP_MAX;
    sem_post(&lq->lock);
    sem_post(&lq->full);
}

int lineup_queue_trypush(lineup_queue_t *lq, order_t client_order){
    if(sem_trywait(&lq->empty) == FAILED) return FAILED;
    sem_wait(&lq->lock);
    lq->buffer[lq->fill] = client_order;
    lq->fill = (lq->fill + 1)%LINEUP_MAX;
    sem_post(&lq->lock);
    sem_post(&lq->full);

    return SUCCESSFUL;
}

//return 0 if pop successful, -1 if nothing to pop
void lineup_queue_pop(lineup_queue_t *lq, order_t *client_order_p){
    sem_wait(&lq->full);
    sem_wait(&lq->lock);
    *client_order_p = lq->buffer[lq->use];
    lq->use = (lq->use + 1) % LINEUP_MAX;
    sem_post(&lq->lock);
    sem_post(&lq->empty);
}

//return 0 if pop successful, -1 if nothing to pop
int lineup_queue_trypop(lineup_queue_t *lq, order_t *client_order_p){
    if(sem_trywait(&lq->full) == FAILED) return FAILED;
    sem_wait(&lq->lock);
    *client_order_p = lq->buffer[lq->use];
    lq->use = (lq->use + 1) % LINEUP_MAX;
    sem_post(&lq->lock);
    sem_post(&lq->empty);

    return SUCCESSFUL;
}
/*


void* cashier_serve_client(void *arg){
    int temp, status;
    time_t rawtime;
    struct tm * timeinfo;

    while((counter_get_c(&no_served)) != CLIENTS){
        status = lineup_queue_trypop(&b, &temp);
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
        if((lineup_queue_trypush(&b, (int)arg)) == FAILED){
            waiting_time = (rand() % LINEUP_MAX_WAITING_TIME) + 1;
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

    lineup_queue_init(&b);
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

    lineup_queue_cleanup(&b);
    counter_cleanup(&no_served);
    return 0;
}

*/

#endif

//end
