#ifndef COUNTER_H
#define COUNTER_H


#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>




typedef struct __counter_t{
  int value;
  sem_t lock;
} counter_t;


void counter_init(counter_t *c){
  c->value = 0;
  sem_init(&(c->lock), 1, 1);
}

void counter_cleanup(counter_t *c){
  sem_destroy(&c->lock);
}
void counter_increment(counter_t *c){
  sem_wait(&c->lock);
  c->value++;
  sem_post(&c->lock);
}

void counter_increment_by_n(counter_t *c, int n){
  sem_wait(&c->lock);
  c->value = c->value + n;
  sem_post(&c->lock);
}

void counter_decrement(counter_t *c){
    sem_wait(&c->lock);
    c->value--;
    sem_post(&c->lock);
}

void counter_decrement_by_n(counter_t *c, int n){
    sem_wait(&c->lock);
    c->value = c->value - n;
    sem_post(&c->lock);
}

int counter_get_c(counter_t *c){
  sem_wait(&c->lock);
  int rc = c->value;
  sem_post(&c->lock);
  return rc;
}


/*

int main(){
  counter_t c;
  counter_init(&c);
  counter_increment(&c);
  printf("counter val: %d\n", counter_get_c(&c));
  counter_decrement(&c);
  printf("counter val: %d\n", counter_get_c(&c));
  counter_cleanup(&c);
  return 0;
}
*/
#endif


//end
