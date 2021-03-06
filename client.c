#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

#include "common.h"
#include "shared_mem.h"
#include "main_token_system.h"
#include "menu.h"


/*
A client has an order and a specified amoutn of time they'll take to it in the
restaurant

Upon attaching the shared memory to its address space, a client attempts
to enter the restaurant, a client will wait until they can enter the restaurant
since the maximum number of pple the restaurant can hold is limited by the
number of tokens that can be issued.
However, if a client enters the restaurant and they find that the queue is full
they'll immediately return the token and leave

A client times the total amount of time they spent inside the restaurant and the
total amount of time they spent waiting in line

Upon entering the lineup queue, ie able to write to the queue buffer, a client
waits upon the token_id until its signaled by the staff at each step of giving the
order till they receive the food

Once they receive the food and eat it, they leave the restaurant thus
returning the token. 

Note the token_id is the abstraction upon which a client coordinates its activities
with the restaurant staff, ie cashiers and servers.



*/

int main(int argc, char *argv[]){

    int status;
    int item_id = 1;

    int is_served = 0;
    int time_before_enter;
    int time_eat = 1;
    int staff_id;
    char item_name[24];
    time_t now;
    struct tm *tm;
    int t1, t2, t3;
    shared_mem_t *sm_p;
    main_token_system_t *mt_p;
    lineup_queue_t *lq_p;
    stats_clients_buf_t *st_c_p;
    menu_item_t *mn_p;
    order_t o;
    o.client_id = getpid();

    sm_p = shared_memory_child_get();
    //printf("Hello from client %d\n", getpid());
    if(sm_p == NULL){
        fprintf(stderr, "Error Cashier: %d unable to attach shared memory.\n\
                        Note, main should create shared memory first. Exit\n", o.client_id);
        exit(1);
    }
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-i") == 0){
            item_id = strtol(argv[i+1], NULL, 0);
        }
        if (strcmp(argv[i], "-e") == 0){
            time_eat = strtol(argv[i+1], NULL, 0);
        }
    }

    mt_p = &(sm_p->mt);
    lq_p = &(sm_p->lq);
    st_c_p = &(sm_p->st_c);
    o.item_id = item_id;
    o.token_id = mt_token_get(mt_p, o.client_id);
     //= menu_get_name(sm_p->mn, o.item_id);
    strcpy(item_name, menu_get_name(sm_p->mn, o.item_id));
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
    now = time(0);
    tm = localtime (&now);//assert not null or sth
    t1 = tm->tm_sec;
    //access shared memory using sm_p
    fprintf(stdout, "Client: %d got token %d entered restaurant\n", o.client_id, o.token_id);
    mt_token_wait_for_signal(mt_p, o.token_id, o.client_id);
    //enter queue
    status = lineup_queue_trypush(lq_p, o);
    if(status == SUCCESSFUL){
        fprintf(stdout, "Client: %d entered queue successfully\n", o.client_id);
        is_served = 1;

        staff_id = mt_token_wait_for_signal(mt_p, o.token_id, o.client_id);
        fprintf(stdout, "Client: %d order for item %d received by Cashier: %d\n", o.client_id, o.item_id, staff_id);

        staff_id = mt_token_wait_for_signal(mt_p, o.token_id, o.client_id);
        now = time(0);
        tm = localtime (&now);//assert not null or sth
        t2 = tm->tm_sec;
        fprintf(stdout, "Client: %d order for item %d processed by Cashier: %d\n", o.client_id, o.item_id, staff_id);

        staff_id = mt_token_wait_for_signal(mt_p, o.token_id, o.client_id);
        fprintf(stdout, "Client: %d received item %d from Server: %d\n", o.client_id, o.item_id, staff_id);

        sleep(time_eat);
        fprintf(stdout, "Client: %d took time %d to eat item: %d name: %s\n", o.client_id, time_eat, o.item_id, item_name);
    }else{
        now = time(0);
        tm = localtime (&now);//assert not null or sth
        t2 = tm->tm_sec;
        fprintf(stdout, "Client: %d restaurant queue full\n", o.client_id);
    }

    //restore value
    mt_token_give_signal(mt_p, o.token_id, o.client_id);
    mt_token_return(mt_p, o.token_id, o.client_id);
    fprintf(stdout, "Client: %d departing from restaurant\n", o.client_id);
    now = time(0);
    tm = localtime (&now);//assert not null or sth
    t3 = tm->tm_sec;
    stats_clients_record(st_c_p, o.client_id, t2-t1, t3-t1, is_served);
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
    // get random eat time rand()

    //write to buffer(client_id, time_spent_total, time_waiting in line )
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
    //detach from shared memory, parent will cleanup
    if(shared_memory_child_detach(sm_p) != SUCCESSFUL){
        fprintf(stderr, "Error Client: %d unable detach shared memory\n", o.client_id);
    }
    return 0;
}
