
#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include "token.h"
#include "common.h"

typedef struct{
    int x;
    main_token_system_t mt;
} shared_mem_t;


void shared_memory_init(shared_mem_t* sm){
    mt_token_system_init(&(sm->mt));
}

void shared_memory_cleanup(shared_mem_t* sm){
    mt_token_system_cleanup(&(sm->mt));
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
