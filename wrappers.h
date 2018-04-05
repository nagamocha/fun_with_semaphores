#include <pthread.h>
#include <stdlib.h>


#ifndef WRAPPERS_H
#define WRAPPERS_H
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

void Pthread_cond_init(pthread_cond_t *cond){
    if(pthread_cond_init(cond, 0) != 0){
      perror("pthread_cond_init() error");
      exit(1);
    }
}

void Pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex){
    if(pthread_cond_wait(cond, mutex) != 0){
      perror("pthread_cond_init() error");
      exit(1);
    }
}

void Pthread_cond_signal(pthread_cond_t *cond){
    if(pthread_cond_signal(cond) != 0){
      perror("pthread_cond_signal() error");
      exit(1);
    }
}

void Pthread_cond_broadcast(pthread_cond_t *cond){
    if(pthread_cond_broadcast(cond) != 0){
      perror("Pthread_cond_broadcast() error");
      exit(1);
    }
}

void Pthread_cond_destroy(pthread_cond_t *cond){
    if(pthread_cond_destroy(cond) != 0){
      perror("pthread_cond_destroy() error");
      exit(1);
    }
}

#endif
