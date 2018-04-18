#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "shared_mem.h"

/*
This is the cleanup program
It cleans up all the values on the shared memory that need to be destroyed
eg semaphores

It then detaches and deletes the shared memory 

*/




int main(){
    int sm_id, status;

    shared_mem_t* sm_p;
    sm_p = shared_memory_child_get();
    sm_id = sm_p->sm_id;

    shared_memory_cleanup(sm_p);

    status = shared_memory_main_detach(sm_p, sm_id);
    if(status < 0){
        printf("Error: Main cleanup\n");
    }else{
        printf("Success Main cleanup\n");
    }
    return 0;
}
