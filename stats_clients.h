#ifndef STATS_CLIENTS_H
#define STATS_CLIENTS_H

#include "counter.h"

/*
functions for client statistics
plus helper functions

*/ 

typedef struct{
    counter_t total_served;
    counter_t total_entered;
    counter_t total_time_waiting;
    counter_t total_time_total;
} stats_clients_buf_t;

void stats_clients_init(stats_clients_buf_t *st_c){
    counter_init(&(st_c->total_served));
    counter_init(&(st_c->total_entered));
    counter_init(&(st_c->total_time_waiting));
    counter_init(&(st_c->total_time_total));
}

void stats_clients_cleanup(stats_clients_buf_t *st_c){
    counter_cleanup(&(st_c->total_served));
    counter_cleanup(&(st_c->total_entered));
    counter_cleanup(&(st_c->total_time_waiting));
    counter_cleanup(&(st_c->total_time_total));
}



void stats_clients_record(stats_clients_buf_t *st_c,int u_client_id, int u_time_waiting, int u_time_total, int u_is_served ){
    if(u_is_served) counter_increment(&(st_c->total_served));
    counter_increment(&(st_c->total_entered));
    counter_increment_by_n(&(st_c->total_time_waiting), u_time_waiting);
    counter_increment_by_n(&(st_c->total_time_total), u_time_total);
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
