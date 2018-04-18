

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "common.h"
#include "lineup_queue.h"
#include "main_token_system.h"
#include "shared_mem.h"
#include "menu.h"

shared_mem_t *sm_p;
int server_id;
menu_item_t *mn_p;
main_token_system_t *mt_p;
int time_serve;
char* msg;

int staff_index;
pid_t* staff_id_arr;

void handle_sig_USR1(int signal);

void cleanup_exit(char* msg, int exit_status);

int register_r(pid_t *staff_id_arr );


void* serve_order(void* arg){
    int time_taken, min, max;
    int item_id, token_id, client_id;
    order_t *o_p = (order_t *)arg;
    item_id = o_p->item_id;
    token_id = o_p->token_id;
    client_id = o_p->client_id;
    sleep(time_serve);
    printf("Server: %d received order for item %d took time %d for Client: %d\n", server_id, item_id, time_serve, client_id);
    min = menu_get_time_min(mn_p, item_id);
    max = menu_get_time_max(mn_p, item_id);
    time_taken = (rand() % (max - min + 1)) + min;
    sleep(time_taken);
    printf("Server: %d served item %d to Client: %d time for preparation: %d\n", server_id, item_id, client_id, time_taken);
    mt_token_give_signal(mt_p, token_id, server_id);
    free(arg);
    return NULL;
}

int main(int argc, char* argv[]){
    int status;
    int token_id;
    lineup_queue_t *sq_p;
    order_t *c_p;
    pthread_t tid;

    signal(SIGUSR1, handle_sig_USR1);
    time_serve = 3;
    server_id = getpid();
    sm_p = shared_memory_child_get();
    //printf("Hello from client %d\n", getpid());
    if(sm_p == NULL){
        fprintf(stderr, "Error Server: %d unable to attach shared memory.\n\
                        Note, main should create shared memory first. Exit\n", server_id);
        exit(1);
    }
    //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-s") == 0){ /*depth of tree*/
            time_serve = strtol(argv[i+1], NULL, 0);
        }
    }
    //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
    //get time break randomly , from argc argv
    //get time_process_order raandomly, from argc argv
    time_serve = (rand() % time_serve) + 1;
    sq_p = &(sm_p->sq);
    mt_p = &(sm_p->mt);
    mn_p = sm_p->mn;
    staff_id_arr = sm_p->staff_id;
    status = register_r(staff_id_arr);
    if(status == FAILED){
        msg = "Failed to register() staff";
        cleanup_exit(msg, 1);
    }
    printf("Server: %d ready to server order\n", server_id);
    while(1){
        c_p = malloc(sizeof(order_t));
        assert(c_p != NULL);
        lineup_queue_pop(sq_p, c_p);
        pthread_create(&tid, 0, serve_order, (void *)c_p);
    }

    //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

    //detach from shared memory, parent will cleanup
    msg="exit";
    cleanup_exit(msg, 0);
}



void handle_sig_USR1(int signal) {
    char* msg = "Server: USR1 signal received, cleanup";
    cleanup_exit(msg, 0);
}

void cleanup_exit(char* msg, int exit_status){
    staff_id_arr[staff_index] = 0;
    fprintf(stderr, "%s\n", msg);
    if(shared_memory_child_detach(sm_p) != SUCCESSFUL){
        fprintf(stderr, "Error Server: %d unable detach shared memory\n", server_id);
    }
    exit(exit_status);
}

int register_r(pid_t *staff_id_arr){
    int i;
    for(i = 0; i < TOTAL_STAFF_SIZE; ++i){
        if (staff_id_arr[i] == 0){
            staff_id_arr[i] = getpid();
            staff_index = i;
            return SUCCESSFUL;
        }
    }
    return FAILED;
}
