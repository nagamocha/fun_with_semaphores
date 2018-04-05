#include <pthread.h>
#include <stdlib.h>

//wrappers
void Pthread_mutex_init(pthread_mutex_t *c){
  if(pthread_mutex_init(c, NULL) != 0){
    perror("pthread_mutex_init() error");
    exit(1);
  }
}
void Pthread_mutex_lock(pthread_mutex_t *c){
  if(pthread_mutex_lock(c)!=0){
    perror("pthread_mutex_lock() error");
    exit(1);
  }
}

void Pthread_mutex_unlock(pthread_mutex_t *c){
  if(pthread_mutex_unlock(c)!=0){
    perror("pthread_mutex_unlock() error");
    exit(1);
  }
}

void Pthread_mutex_destroy(pthread_mutex_t *c){
  if(pthread_mutex_destroy(c) != 0){
    perror("pthread_mutex_destroy() error");
    exit(1);
  }
}
