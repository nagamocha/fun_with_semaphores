
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <semaphore.h>

#include "common.h"


int main(int argc, char *argv[]){
  key_t shm_key;
  int shm_id;
  s_memory_cw *shd_mem_cw_p;
  int status;

  shm_key = ftok(".", 'x');
  shm_id = shmget(shm_key, sizeof(s_memory_cw), 0666);
  if(shm_id < 0){
    printf("shmget error client\n");
    exit(1);
  }
  //attach memory to own address space
  shd_mem_cw_p= (s_memory_cw *)shmat(shm_id, NULL, 0);
  if((long) shd_mem_cw_p== -1){
    printf("shmat() error client\n");
    exit(1);
  }

  int *arr = shd_mem_cw_p->arr;
  sem_t *sem_p = &(shd_mem_cw_p->sem);
  arr[0] = 90;
  arr[1] = 100;
  arr[2] = 120;
  arr[3] = 300;

  status = sem_post(sem_p);
  if(status != 0){
    printf("error sem_post() client\n");
  }


  //detach shared memory from address space
  status = shmdt(shd_mem_cw_p);
  if(status < 0){
    printf("error Shared memory cw detachment, client\n");
  }
  //parent will remove the segment

  return 0;
}
