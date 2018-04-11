

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include "common.h"
#include "lineup_queue.h"
#include "main_token_system.h"
#include "shared_mem.h"
#include "stats_purchases.h"
#include "menu.h"

shared_mem_t *sm_p;
int cashier_id;

void handle_sig_USR1(int signal) {
    if(shared_memory_child_detach(sm_p) != SUCCESSFUL){
        fprintf(stderr, "Error Cashier: %d unable detach shared memory\n", cashier_id);
    }
}

int main(int argc, char* argv[]){
    int status;

    int token_id;
    int time_process_order = 3;
    int time_break = 10;
    lineup_queue_t *lq_p;
    main_token_system_t *mt_p;
    order_t c;

    signal(SIGUSR1, handle_sig_USR1);
    cashier_id = getpid();
    sm_p = shared_memory_child_get();
    //printf("Hello from client %d\n", getpid());
    if(sm_p == NULL){
        fprintf(stderr, "Error Cashier: %d unable to attach shared memory.\n\
                        Note, main should create shared memory first. Exit\n", cashier_id);
        exit(1);
    }
    //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-s") == 0){ /*depth of tree*/
            time_process_order = strtol(argv[i+1], NULL, 0);
        }
        if (strcmp(argv[i], "-b") == 0){ /*depth of tree*/
            time_break = strtol(argv[i+1], NULL, 0);
        }
    }
    //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
    //get time break randomly , from argc argv
    //get time_process_order raandomly, from argc argv
    time_break = (rand() % time_break) + 1;
    time_process_order = (rand() % time_process_order) + 1;
    lq_p = &(sm_p->lq);
    mt_p = &(sm_p->mt);

    while(1){
        status = lineup_queue_trypop(lq_p, &c);
        printf("Cashier: %d ready to receive order\n", cashier_id);
        if(status == SUCCESSFUL){
            //receive order
            printf("Cashier: %d received order item %d from Client: %d\n", cashier_id, c.item_id, c.client_id);
            mt_token_give_signal(mt_p, c.token_id, cashier_id);
            //process order
            sleep(time_process_order);
            printf("Cashier: %d processed order item %d from Client: %d, time taken: %d\n", cashier_id, c.item_id, c.client_id, time_process_order);
            mt_token_give_signal(mt_p, c.token_id, cashier_id);
            //record, accounting purchase
            stats_purchase_record(sm_p->st_p, c.item_id);
            lineup_queue_push(&(sm_p->sq), c);//push to server
        }else{
            printf("Cashier: %d Queue empty, going for a break for %d seconds\n", cashier_id, time_break);
            sleep(time_break);
        }
    }

    //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

    //detach from shared memory, parent will cleanup
    if(shared_memory_child_detach(sm_p) != SUCCESSFUL){
        fprintf(stderr, "Error Cashier: %d unable detach shared memory\n", cashier_id);
    }
    return 0;
}






































//end
