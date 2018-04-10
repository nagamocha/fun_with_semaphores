
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


#include "common.h"
#include "shared_mem.h"
#include "token.h"

int main(){
    int status;
    int pid, i, cid;
    char* myargs[2];

    int client_id = getpid();
    int token_id;

    int sm_id;
    shared_mem_t *sm_p;

    sm_p = shared_memory_main_get(&sm_id);
    if(sm_p == NULL){
        fprintf(stderr, "error getting shared memory main_process exiting...\n");
        exit(1);
    }
    shared_memory_init(sm_p);
    main_token_system_t *mt_p;

    mt_p = &(sm_p->mt);
    token_id = mt_token_get(mt_p, client_id);
    //access shared memory using sm_p
    fprintf(stdout, "Client: %d got token %d\n", client_id, token_id);
    mt_token_wait_for_signal(mt_p, token_id, client_id);
    fprintf(stderr, "Client: %d received signal\n", client_id);
    mt_token_return(mt_p, token_id, client_id);
    printf("Client %d returned token\n", getpid());



/*
    cid = fork();
    switch(cid){
        case -1:
            printf("error fork()\n");
            break;
        case 0://child
            //sprintf(s_root_pid,"%d",root_pid);
            myargs[0] = "./client";
            myargs[1] = NULL;
            execvp(myargs[0], myargs);
            break;//not supposed to reach here
        default: //MAIN
            waitpid(cid, &status, 0);
            printf("child %d exit with status: %d\n", cid, status);
            printf("int x = %d\n", sm_p->x);


    }*/

    shared_memory_cleanup(sm_p);
    status = shared_memory_main_detach(sm_p, sm_id);
    return 0;
}
