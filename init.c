#include <stdio.h>
#include <stdlib.h>
#include "shared_mem.h"


/*
Before the restaurant starts running. this program should be run
as it requests for the shared memory and initializes all the datastrucutres in
the main memory appropriately

*/


int main(){
    int sm_id;
    shared_mem_t *sm_p;

    sm_p = shared_memory_main_get(&sm_id);
    if(sm_p == NULL){
        fprintf(stderr, "error getting shared memory main_process. exit\n");
        exit(1);
    }
    shared_memory_init(sm_p);

    sm_p->sm_id = sm_id;
    return 0;
}



































//end
