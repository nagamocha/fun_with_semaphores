#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>

#include "common.h"
#include "shared_mem.h"
#include "token.h"

int main(int argc, char *argv[]){

    int status;
    int token_id = -1;
    int client_id = getpid();
    shared_mem_t *sm_p;
    main_token_system_t *mt_p;

    client_id = getpid();
    sm_p = shared_memory_child_get();
    printf("Hello from client %d\n", getpid());
    if(sm_p == NULL){
        fprintf(stderr, "Error Client: %d unable to attach shared memory, exit\n", client_id);
        exit(1);
    }
    int *j;
    j = &(sm_p->x);
    *j =90;

    mt_p = &(sm_p->mt);
    token_id = mt_token_get(mt_p, client_id);
    printf("zoom\n");
    //access shared memory using sm_p
    fprintf(stdout, "Client: %d got token %d\n", client_id, token_id);
    mt_token_wait_for_signal(mt_p, token_id, client_id);
    fprintf(stderr, "Client: %d received signal\n", client_id);
    mt_token_return(mt_p, token_id, client_id);
    printf("Zzup from client %d\n", getpid());

    //detach from shared memory, parent will cleanup
    if(shared_memory_child_detach(sm_p) != SUCCESSFUL){
        fprintf(stderr, "Error Client: %d unable detach shared memory\n", client_id);
    }
    return 0;
}
