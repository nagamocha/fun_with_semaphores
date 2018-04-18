
#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include "common.h"
#include "main_token_system.h"
#include "lineup_queue.h"
#include "menu.h"
#include "stats_purchases.h"
#include "stats_clients.h"


/*

shared memory consists of the following
main_token_system_t mt;
see main_token_system.h for further details

lineup_queue_t lq;
the lineup queue, bounded buffer for all the orders that clients write and
cashiers read

lineup_queue_t sq;
the server queue, bounded buffer for all the orders that cashiers pass to the
server

menu_item_t mn[TOTAL_MENU_ITEMS];
stores all the menu items, eg for item 1, the name, preparation time, price etc

counter_t st_p[TOTAL_MENU_ITEMS];
stores all the purches stats, eg how much of item 3 was purchased that day

stats_clients_buf_t st_c;
stores all the client stats, ie total time waiting, total number of clients
that visited etc

pid_t staff_id[TOTAL_STAFF_SIZE];
the process ids of all the staff members

int sm_id;
the shared memory id of the shared memory


Shared memory also provides functions for inititializng the datastructures above
and cleabing up after done
*/


typedef struct{
    main_token_system_t mt;
    lineup_queue_t lq;
    lineup_queue_t sq;
    menu_item_t mn[TOTAL_MENU_ITEMS];
    counter_t st_p[TOTAL_MENU_ITEMS];
    stats_clients_buf_t st_c;
    pid_t staff_id[TOTAL_STAFF_SIZE];
    int sm_id;
} shared_mem_t;


void shared_memory_init(shared_mem_t* sm){
    int i;
    mt_token_system_init(&(sm->mt));
    lineup_queue_init(&(sm->lq));
    lineup_queue_init(&(sm->sq));
    stats_purchase_init(sm->st_p);
    stats_clients_init(&(sm->st_c));
    menu_init(sm->mn);
    for(i = 0; i < TOTAL_STAFF_SIZE; ++i){
        sm->staff_id[i] = 0;
    }
}

void shared_memory_cleanup(shared_mem_t* sm){
    mt_token_system_cleanup(&(sm->mt));
    lineup_queue_cleanup(&(sm->lq));
    lineup_queue_cleanup(&(sm->sq));
    stats_purchase_cleanup(sm->st_p);
    stats_clients_cleanup(&(sm->st_c));
}

shared_mem_t* shared_memory_child_get(){
    shared_mem_t* sm_p = NULL;
    key_t sm_key;
    int sm_id;
    int status;

    sm_key = ftok(".", 'x');
    sm_id = shmget(sm_key, sizeof(shared_mem_t), 0666);
    if(sm_id < 0){
      fprintf(stderr, "shmget error child\n");
      return NULL;
    }
    sm_p = (shared_mem_t *)shmat(sm_id, NULL, 0);
    if((long) sm_p== -1){
      fprintf(stderr, "shmat() error child\n");
      return NULL;
    }
    return sm_p;
}

shared_mem_t* shared_memory_main_get(int* sm_id_p){
    shared_mem_t* sm_p = NULL;
    key_t sm_key;
    int sm_id;
    int status;

    sm_key = ftok(".", 'x');
    //request for shared memory
    *sm_id_p = shmget(sm_key, sizeof(shared_mem_t), IPC_CREAT|0666);
    if(*sm_id_p < 0){
      fprintf(stderr, "error shmget() main_process\n");
      return NULL;
    }

    //attach s_memory to own address space
    sm_p = (shared_mem_t *)shmat(*sm_id_p, NULL, 0);
    if((long) sm_p == -1){
      fprintf(stderr, "error shmat() main_process\n");
      return NULL;
    }

    return sm_p;
}


int shared_memory_main_detach(shared_mem_t* sm, int sm_id){
    int status;
    //detach shared s_memory from address space
    status = shmdt(sm);
    if(status < 0){
        fprintf(stderr, "error Shared memory detachment, Main\n");
    }
     //remove shared s_memory segment
    status = shmctl(sm_id, IPC_RMID, NULL);
    if(status < 0){
        fprintf(stderr, "error Shared memory removal, Main\n");
    }

    return status;
}


int shared_memory_child_detach(shared_mem_t* sm){
    //detach shared memory from address space
    return shmdt(sm);
}


#endif
