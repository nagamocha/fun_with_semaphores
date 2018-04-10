#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>

#include "counter.h"
#include "wrappers.h"

#define MAX 100
#define LOOPS 1
#define CONSUMERS 10
#define PRODUCERS 10

int buffer[MAX];
int fill = 0;
int use = 0;
volatile int end = 0;

sem_t empty;
sem_t full;
sem_t mutex;

counter_t c;



void insert(int value){
    //printf("v: %d\n", value);
    buffer[fill] = value;
    fill = (fill + 1) % MAX;
}

int get(){
    int tmp = buffer[use];
    use = (use + 1) % MAX;
    return tmp;
}

void* producer(void *arg){
    int i, temp;
    for(i = 0; i < LOOPS; ++i){
        increment(&c);
        temp = get_c(&c);

        sem_wait(&empty);
        sem_wait(&mutex);
        insert(temp);
        sem_post(&mutex);
        sem_post(&full);
    }

    return NULL;
}

void* consumer(void *arg){
    int i, temp;
    void* ret_val_p;

    while(1){
        sem_wait(&full);
        sem_wait(&mutex);
        temp = get();
        sem_post(&mutex);
        sem_post(&empty);
        printf("C: %d, %d\n", temp, end);

        //if(end == 1){
        //    return NULL;
        //}
    }

    return NULL;
}

int main(){
    int i;
    pthread_t pd[PRODUCERS];
    pthread_t cm[CONSUMERS];

    sem_init(&empty, 0, MAX);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);

    init_counter(&c);




    for(i = 0; i < PRODUCERS; ++i){
        pthread_create(pd+i, 0, producer, 0);
    }

    for(i = 0; i < CONSUMERS; ++i){
        pthread_create(cm+i, 0, consumer, 0);
    }

    for(i = 0; i < PRODUCERS; ++i){
        pthread_join(pd[i], NULL);
    }


    end = 1;
    for(i = 0; i < CONSUMERS; ++i){
        pthread_join(cm[i], NULL);
    }
    return 0;
}




























































//end
