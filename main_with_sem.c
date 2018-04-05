
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <semaphore.h>

#include "common.h"


int main(){
  sem_t *sem_p;
  key_t shm_key;
  int shm_id, pid, status,cid, i;
  s_memory_cw *shd_mem_cw_p;
  char* myargs[2];





  shm_key = ftok(".", 'x');
  //request for shared memory
  shm_id = shmget(shm_key, sizeof(s_memory_cw), IPC_CREAT|0666);
  if(shm_id < 0){
    printf("error shmget() main_process\n");
    exit(1);
  }

  //attach s_memory to own address space
  shd_mem_cw_p= (s_memory_cw *)shmat(shm_id, NULL, 0);
  if((long) shd_mem_cw_p== -1){
    printf("error shmat() main_process\n");
    exit(1);
  }

  //initialize semaphore
  sem_p = &(shd_mem_cw_p->sem);
  status = sem_init(sem_p, 1, 0);
  if(status != 0){
    printf("erro sem_init() sem cw\n");
    exit(1);
  }

  //fork child, child writes to shared s_memory
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
    default:
      status = sem_trywait(sem_p);
      printf("Did try wait, status: %d\n", status);
      if(status == -1){
        //do proper wait
        status = sem_wait(sem_p);
        printf("Did wait, status: %d\n", status);
      }

      int *arr = shd_mem_cw_p->arr;
      for(i = 0; i < 4; ++i){
          printf("%d ", arr[i]);
      }
      printf("\n");

  }

  status = sem_destroy(sem_p);
  if(status < 0){
    printf("error sem_destroy sem cw, Main\n");
  }

  //detach shared s_memory from address space
  status = shmdt(shd_mem_cw_p);
  if(status < 0){
    printf("error Shared memory cw detachment, Main\n");
  }
  //remove shared s_memory segment
  status = shmctl(shm_id, IPC_RMID, NULL);
  if(status < 0){
    printf("error Shared memory cw removal, Main\n");
  }



  return 0;
}
