#include "wrappers.h"
typedef struct{
    int status
    pthread_mutex_t lock;
    pthread_cond_t cond;
} Eof;

void init_eof(Eof *eof_p){
    eof_p->status = 0;
    Pthread_mutex_init(eof_p->lock);
    Pthread_cond_init(eof_p->cond);
}

void clean_up_eof(Eof *eof_p){
    Pthread_mutex_destroy(eof_p->lock);
    Pthread_cond_destroy(eof_p->cond);
}

void send_eof(Eof *eof_p){
    Pthread_mutex_lock(eof_p->lock);
    eof_p->status = 1;
    Pthread_cond_broadcast(eof_p->cond);
    Pthread_mutex_unlock(eof_p->lock);
}
