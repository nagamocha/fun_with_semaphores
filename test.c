
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>


#include "common.h"
#include "shared_mem.h"
#include "main_token_system.h"
#include "menu.h"
#include "stats_purchases.h"
#include "stats_clients.h"
/*
Test program for testing the entire restaurant system eg,
launches cashiers, server

It then launches the clients

After every client has exited
It reports the restaurant stats  and exits


*/

int main(){
    int status, total_clients, total_served;
    int i, cid;
    pid_t client_pids[CLIENTS];
    char* myargs[6];
    shared_mem_t *sm_p;
    float revenue, ave_waiting_time;
    stats_clients_buf_t *stat_clients_p;


    main_token_system_t *mt_p;
    pid_t* staff_id_arr;

    sm_p = shared_memory_child_get();

    mt_p = &(sm_p->mt);
    stat_clients_p = &(sm_p->st_c);
    staff_id_arr = sm_p->staff_id;

    myargs[0] = "./cashier";
    myargs[1] = NULL;
    //launch_cashier
    for(i = 0; i < 3; ++i){
        cid = fork();
        if(cid == -1){
            perror("error fork()\n");
            break;
        }else if(cid == 0){
            execvp(myargs[0], myargs);
            exit(9);//not supposed to reach here
        }else   continue;//spawn more clients
    }
    //launch server
    myargs[0] = "./server";
    myargs[1] = NULL;
    cid = fork();
    if(cid == -1){
        perror("error fork()\n");
    }else if(cid == 0){
        execvp(myargs[0], myargs);
        exit(9);//not supposed to reach here
    }
    //-------------------------------------------------------------------------
    //launch clients
    myargs[0] = "./client";
    myargs[1] = "-i";
    myargs[2] = "1";
    myargs[3] = "-e";
    myargs[4] = "1";
    myargs[5] = NULL;
    for(i = 0; i < CLIENTS; ++i){
        cid = fork();
        if(cid == -1){
            perror("error fork()\n");
            break;
        }else if(cid == 0){
            execvp(myargs[0], myargs);
            exit(9);//not supposed to reach here
        }else  client_pids[i] = cid;//spawn more clients
    }
    //reap children
    for(i = 0; i < CLIENTS; ++i){
        cid = waitpid(client_pids[i], &status, 0);
        printf("child %d exit with status: %d\n", cid, status);
    }

    //-------------------------------------------------------------------------
    //kill staff
    sleep(10);
    for(i = 0; i < TOTAL_STAFF_SIZE; ++i){
        if (staff_id_arr[i] != 0){
            kill(staff_id_arr[i], SIGUSR1);
        }
    }
    //-------------------------------------------------------------------------
    //report stats
    revenue = stats_purchase_total_revenue(sm_p->st_p, sm_p->mn);
    total_clients = stats_clients_entered_sum(stat_clients_p);
    total_served = stats_clients_served_sum(stat_clients_p);
    ave_waiting_time = stats_clients_ave_time_waiting(stat_clients_p);
    printf("TOTAL REVENUE:        %f\n", revenue);
    printf("CLIENTS ENTERED:      %d\n", total_clients);
    printf("CLIENTS SERVED:       %d\n", total_served);
    printf("AVERAGE WAITING TIME: %f\n", ave_waiting_time);

    status = shared_memory_child_detach(sm_p);


    return 0;
}
