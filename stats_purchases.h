#ifndef STATS_PURCHASE_H
#define STATS_PURCHASE_H

#include "counter.h"
#include "menu.h"
/*
functions for recording/reporting purchases
plus helper functions


*/
void stats_purchase_init(counter_t *st_p){
    int i;
    for(i = 0; i < TOTAL_MENU_ITEMS; ++i){
        counter_init(st_p + i);

    }
}

void stats_purchase_cleanup(counter_t *st_p){
    int i;
    for(i = 0; i < TOTAL_MENU_ITEMS; ++i){
        counter_cleanup(st_p + i);
    }
}

void stats_purchase_record(counter_t *st_p, int item_id){
    counter_increment(st_p + (item_id -1));
}

float stats_purchase_total_revenue(counter_t *st_p, menu_item_t *mn){
    int i;
    float revenue = 0;
    for(i = 0; i < TOTAL_MENU_ITEMS; ++i){
        revenue += ((counter_get_c(st_p + i)) * menu_get_value(mn, i + 1));
    }
    return revenue;
}
































#endif
