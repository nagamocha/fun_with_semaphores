
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>


#include "common.h"
#include "shared_mem.h"
#include "main_token_system.h"
#include "menu.h"
#include "stats_purchases.h"
#include "stats_clients.h"

#define NO 9

typedef struct{
    stats_clients_buf_t *st_c_p;
    FILE* fp;
    int continue_running;
}collect_client_stats_args_t;

void* collect_client_stats(void* arg){
    int status;
    stats_clients_buf_t *st_c_p = ((collect_client_stats_args_t *)arg)->st_c_p;
    FILE* stats_clients_fp = ((collect_client_stats_args_t *)arg)->fp;
    stats_client_t s ;
    while(1){
        status = ((collect_client_stats_args_t *)arg)->continue_running;
        //stats_clients_pop(st_c_p, &s);
        //fwrite(&s, sizeof(stats_client_t), 1, stats_clients_fp);
        if (status == 0) break;
    }
    return NULL;
}

int main(){
    int status;
    int pid, i, cid;
    char* myargs[2];

    int client_id = getpid();
    int token_id;

    int sm_id;
    shared_mem_t *sm_p;
    main_token_system_t *mt_p;
    stats_clients_buf_t *st_c_p;
    collect_client_stats_args_t *ccs_arg_p;
    pthread_t tid;

    FILE* stats_clients_fp = fopen("stats_clients.bin", "wb");
    ccs_arg_p = malloc(sizeof(collect_client_stats_args_t));

    if(!stats_clients_fp || !ccs_arg_p){
        fprintf(stderr, "error setup stats_clients.bin. exit");
        exit(2);
    }

    sm_p = shared_memory_main_get(&sm_id);
    if(sm_p == NULL){
        fprintf(stderr, "error getting shared memory main_process. exit\n");
        exit(1);
    }
    shared_memory_init(sm_p);

    //create thread that handles client stats
    pthread_create(&tid, 0, collect_client_stats, (void *)ccs_arg_p );

    mt_p = &(sm_p->mt);
    st_c_p = &(sm_p->st_c);
    ccs_arg_p->st_c_p = st_c_p;
    ccs_arg_p->fp = stats_clients_fp;
    ccs_arg_p->continue_running = 1;

    myargs[0] = "./client";
    myargs[1] = NULL;

    for(i = 0; i < NO; ++i){
        cid = fork();
        if(cid == -1){
            perror("error fork()\n");
            break;
        }else if(cid == 0){
            execvp(myargs[0], myargs);
            exit(9);//not supposed to reach here
        }else   continue;//spawn more clients
    }

    //reap children
    for(i = 0; i < CLIENTS; ++i){
        cid = wait(&status);
        //printf("child %d exit with status: %d\n", cid, status);
    }
    //collect client stats

    //kill(root_pid, SIGUSR1);

    ccs_arg_p->continue_running = 0;
    status = pthread_join(tid, NULL);
    if(status == SUCCESSFUL){
        printf("OKEY DOKEY\n");
    }else{
        fprintf(stderr, "Error client stats thread unable to collect client stats\n");
    }
    free(ccs_arg_p);
    fclose(stats_clients_fp);
    shared_memory_cleanup(sm_p);
    status = shared_memory_main_detach(sm_p, sm_id);
    return 0;
}
