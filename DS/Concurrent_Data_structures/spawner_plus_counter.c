#include <stdio.h>
#include <pthread.h>
#include  <semaphore.h>
#include <stdio.h>
#include <pthread.h>
#include "wrappers.h"


#define NO_OF_THREADS 20
#define NO_OF_LOOPS 100
sem_t s;
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
typedef struct __counter_t{
  int value;
  pthread_mutex_t lock;
} counter_t;

counter_t c;

void init_counter(counter_t *c){
  c->value = 0;
  Pthread_mutex_init(&c->lock);
}

void increment(counter_t *c){
  Pthread_mutex_lock(&c->lock);
  c->value++;
  Pthread_mutex_unlock(&c->lock);
}

void decrement(counter_t *c){
  Pthread_mutex_lock(&c->lock);
  c->value--;
  Pthread_mutex_unlock(&c->lock);
}

int get(counter_t *c){
  Pthread_mutex_lock(&c->lock);
  int rc = c->value;
  Pthread_mutex_unlock(&c->lock);
  return rc;
}

void clean_up_counter(counter_t *c){
  Pthread_mutex_destroy(&c->lock);
}
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*



void* foo(void* arg){
  int i;
  for(i = 0; i < NO_OF_LOOPS; ++i) increment(&c);
  sem_post(&s);
  return NULL;
}
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

int main(){
  int i;

  init_counter(&c);

  pthread_t thread[NO_OF_THREADS];
  sem_init(&s, 0, -23);
  //spawn threads
  for(i = 0; i < NO_OF_THREADS; ++i){
    pthread_create(&thread[i], NULL, foo, NULL);
  }

  sem_wait(&s);
  printf("DONE :%d\n", get(&c));
  return 0;
}


































































































//end
