#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>

#include "common.h"
#include "shared_mem.h"
#include "main_token_system.h"
#include "menu.h"

int main(int argc, char *argv[]){

    int status;
    int token_id = -1;
    int client_id;
    int item_id;

    int is_served = 0;
    int time_before_enter;
    int time_eat;
    int time_waiting = 0;
    int time_total = 0;
    shared_mem_t *sm_p;
    main_token_system_t *mt_p;

    client_id = getpid();
    sm_p = shared_memory_child_get();
    //printf("Hello from client %d\n", getpid());
    if(sm_p == NULL){
        fprintf(stderr, "Error Cashier: %d unable to attach shared memory.\n\
                        Note, main should create shared memory first. Exit\n", client_id);
        exit(1);
    }

    mt_p = &(sm_p->mt);
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
    token_id = mt_token_get(mt_p, client_id);
    //access shared memory using sm_p
    fprintf(stdout, "Client: %d got token %d\n", client_id, token_id);
    mt_token_wait_for_signal(mt_p, token_id, client_id);
    //fprintf(stderr, "Client: %d received signal\n", client_id);
    //restore value
    mt_token_give_signal(mt_p, token_id, client_id);
    mt_token_return(mt_p, token_id, client_id);
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
    // get random eat time rand()
    //get item_id
    //wait for time_before_enter before trying to enter

    //enter restaurant, get token
    //check queue, if queue full, go to EXIT

    //if queue not full, write to queue your order
    //set is served to true
    //order consists of(item orderd, value, client_id, token_id)

    //waitfor signal from cashier, that cashier_id is has received your order
    //print that shit out

    //after cashier takes their time processing your order
    //wait for second signal that cashier processed your order
    //print that shit out,
    //record total time spent waiting

    //once that, wait for signal from server_id for  food ready
    //print that shit out

    //once food ready, take time t_eat to eat ie just use sleep()
    //then goto EXIT


    //return token, get_time spent, print out that you left
    //write to buffer(client_id, time_spent_total, time_waiting in line )
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
    //detach from shared memory, parent will cleanup
    if(shared_memory_child_detach(sm_p) != SUCCESSFUL){
        fprintf(stderr, "Error Client: %d unable detach shared memory\n", client_id);
    }
    return 0;
}
