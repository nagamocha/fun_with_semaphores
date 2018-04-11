#ifndef STATS_CLIENTS_H
#define STATS_CLIENTS_H

#include "counter.h"

#define STATS_CLIENTS_MAX 3

typedef struct{
    int client_id;
    int time_waiting;
    int time_total;
    int is_served;
} stats_client_t;


typedef struct{
    stats_client_t buffer[STATS_CLIENTS_MAX];
    int fill;
    int use;
    counter_t total_served;
    counter_t total_entered;
    counter_t total_time_waiting;
    counter_t total_time_total;
    sem_t empty;
    sem_t full;
    sem_t lock;
} stats_clients_buf_t;

void stats_clients_init(stats_clients_buf_t *st_c){
    st_c->fill = 0;
    st_c->use = 0;
    counter_init(&(st_c->total_served));
    counter_init(&(st_c->total_entered));
    counter_init(&(st_c->total_time_waiting));
    counter_init(&(st_c->total_time_total));
    sem_init(&st_c->empty, 1, STATS_CLIENTS_MAX);
    sem_init(&st_c->full, 1, 0);
    sem_init(&st_c->lock, 1, 1);
}

void stats_clients_cleanup(stats_clients_buf_t *st_c){
    counter_cleanup(&(st_c->total_served));
    counter_cleanup(&(st_c->total_entered));
    counter_cleanup(&(st_c->total_time_waiting));
    counter_cleanup(&(st_c->total_time_total));
    sem_destroy(&st_c->empty);
    sem_destroy(&st_c->full);
    sem_destroy(&st_c->lock);
}

void stats_clients_push(stats_clients_buf_t *st_c, int u_client_id, int u_time_waiting, int u_time_total, int u_is_served){
    stats_client_t s;
    s.client_id = u_client_id;
    s.time_waiting = u_time_waiting;
    s.time_total = u_time_total;
    s.is_served = u_is_served;
    sem_wait(&st_c->empty);
    sem_wait(&st_c->lock);
    st_c->buffer[st_c->fill] = s;
    st_c->fill = (st_c->fill + 1) % STATS_CLIENTS_MAX;
    sem_post(&st_c->lock);
    sem_post(&st_c->full);
}

void stats_clients_pop(stats_clients_buf_t *st_c, stats_client_t* s_p){
    sem_wait(&st_c->full);
    sem_wait(&st_c->lock);
    *s_p = st_c->buffer[st_c->use];
    st_c->use = (st_c->use + 1) % STATS_CLIENTS_MAX;
    sem_post(&st_c->lock);
    sem_post(&st_c->empty);

    //update stats
    if(s_p->is_served) counter_increment(&(st_c->total_served));
    counter_increment(&(st_c->total_entered));
    counter_increment_by_n(&(st_c->total_time_waiting), s_p->time_waiting);
    counter_increment_by_n(&(st_c->total_time_total), s_p->time_total);
}

int stats_clients_served_sum(stats_clients_buf_t *st_c){
    return counter_get_c(&(st_c->total_served));
}

int stats_clients_entered_sum(stats_clients_buf_t *st_c){
    return counter_get_c(&(st_c->total_entered));
}

float stats_clients_ave_time_waiting(stats_clients_buf_t *st_c){
    int total_served = stats_clients_served_sum(st_c);
    int total_time_waiting = counter_get_c(&(st_c->total_time_waiting));
    return (float) total_time_waiting / (float) total_served;
}





















#endif
