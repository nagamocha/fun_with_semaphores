#include <pthread.h>
#include "wrappers.h"



typedef struct __counter_t{
  int value;
  pthread_mutex_t lock;
} counter_t;


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

int get_c(counter_t *c){
  Pthread_mutex_lock(&c->lock);
  int rc = c->value;
  Pthread_mutex_unlock(&c->lock);
  return rc;
}

void clean_up_counter(counter_t *c){
  Pthread_mutex_destroy(&c->lock);
}


/*
int main(){
  counter_t c;
  init_counter(&c);
  increment(&c);
  printf("counter val: %d\n", get(&c));
  decrement(&c);
  printf("counter val: %d\n", get(&c));
  clean_up_counter(&c);
  return 0;
}
*/





















































//end
