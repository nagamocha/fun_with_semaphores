/*
FIFO token_queue, adapted from OSTEP, section 29.3
Concurrent token_queue
*/

#ifndef MAIN_TOKEN_H
#define MAIN_TOKEN_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "token_queue.h"

/*
The main_token system is the heart of the restaurant system as it is used to
coordinate between the clients and the staff(cashiers and staff)

Basicallly, once a client enters the restaurant, they are issued a token,
A token is basically a semaphore on which a client waits upon as the staff
signal ie post to the client at different junctures eg, when a client's food is ready

Once a client leaves the restaurant, they return the token and the same token can
be used to coordinate with a new client

Whenever, a client is issued a token, they are given an id onto which they can
index into the semaphore array, their pid is also attached to the semaphore so that
a client can only wait upon a semaphore that they were issued issued

Further protections will be added in the future to guarantee that a clients/staff
only wait/post on the appropriate token at a given moment


*/

typedef struct{
    token_queue_t tq;
    sem_t sem_arr[NO_OF_TOKENS];
    int ct_assns[NO_OF_TOKENS];//mapping from index to client id
    int sig_senders[NO_OF_TOKENS];
} main_token_system_t;


//If token available, returns nonnegative number, else returns -1
//client doesnt have to wait
void mt_token_system_init(main_token_system_t *mt);
int mt_token_get(main_token_system_t *mt, int u_client_id);
int mt_token_tryget(main_token_system_t *mt, int u_client_id);
int mt_token_wait_for_signal(main_token_system_t *mt, int index, int u_client_id);
int mt_token_give_signal(main_token_system_t *mt, int index, int sig_sender);
int mt_token_return(main_token_system_t *mt, int index, int u_client_id);
void mt_token_system_cleanup(main_token_system_t *mt);



//FUNCTIONS FOR MAIN TOKEN SYSTEM
void mt_token_system_init(main_token_system_t *mt){
    int i;
    memset(&(mt->ct_assns), -1, NO_OF_TOKENS * sizeof(int));
    token_queue_init(&mt->tq);
    for(i = 0; i < NO_OF_TOKENS; ++i){
        token_queue_push(&mt->tq, i);
        sem_init(&(mt->sem_arr[i]), 1, 1);
    }
}

void mt_token_system_cleanup(main_token_system_t *mt){
    int i;
    token_queue_cleanup(&mt->tq);
    for(i = 0; i < NO_OF_TOKENS; ++i){
        sem_destroy(&(mt->sem_arr[i]));
    }
}


/*
If token available, returns nonnegative number, else
returns -1
*/
int mt_token_get(main_token_system_t *mt, int u_client_id){
    int index =  token_queue_pop(&(mt->tq));
    if(index != -1){
        mt->ct_assns[index] = u_client_id;
    }
    return index;
}

int mt_token_tryget(main_token_system_t *mt, int u_client_id){
    int index;
    index =  token_queue_trypop(&(mt->tq));
    if(index != -1){
        mt->ct_assns[index] = u_client_id;
    }
    return index;
}

//returns sig sender id who sent signal, else returns -1, if
//non-inteded sig receiver attempted to wait on sig sem not assigned to them
int mt_token_wait_for_signal(main_token_system_t *mt, int index, int u_client_id){
    if(u_client_id != (mt->ct_assns[index])) return -1;
    sem_wait(&(mt->sem_arr[index]));
    return mt->sig_senders[index];
}

int mt_token_give_signal(main_token_system_t *mt, int index, int sig_sender){
    mt->sig_senders[index] = sig_sender;
    sem_post(&(mt->sem_arr[index]));
    return 0;
}

int mt_token_return(main_token_system_t *mt, int index, int u_client_id){
    if(u_client_id != (mt->ct_assns[index])) return -1;
    mt->ct_assns[index] = -1;
    token_queue_push(&(mt->tq), index);
    return 0;
}

//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#endif
