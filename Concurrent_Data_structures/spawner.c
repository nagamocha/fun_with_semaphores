#include <stdio.h>
#include <pthread.h>
#include  <semaphore.h>

#define NO_OF_THREADS 10
sem_t s;

typedef struct {} counter_t;

void* foo(void* arg){
  printf("Val: %d \n",(int) arg);
  fflush(stdout);
  sem_post(&s);
  return NULL;
}


int main(){
  int i;
  pthread_t thread[NO_OF_THREADS];
  sem_init(&s, 0, 1);
  //spawn threads
  for(i = 0; i < NO_OF_THREADS; ++i){
    pthread_create(&thread[i], NULL, foo, (void *)i);
    sem_wait(&s);
    printf("spawned: %d\n", i);
  }

  sem_wait(&s);
  printf("DONE\n");

  sem_destroy(&s);
  return 0;
}


































































































//end
