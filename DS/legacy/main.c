
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>



int main(){
  key_t shm_key;
  int shm_id, pid, status,cid, i;
  int *shm_ptr;
  char* myargs[2];
  shm_key = ftok(".", 'x');
  //request for shared memory
  shm_id = shmget(shm_key,  4*sizeof(int), IPC_CREAT|0666);
  if(shm_id < 0){
    printf("shmget error\n");
    exit(1);
  }

  //attach memory to own address space
  shm_ptr = (int *)shmat(shm_id, NULL, 0);
  if((long) shm_ptr == -1){
    printf("shmat() error main_process\n");
    exit(1);
  }


  //fork child, child writes to shared memory
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
      pid = waitpid(cid, &status, 0);
      if(pid != cid){//ie pid < 0
        printf("error waitpid()\n");
        break;
      }
      if(WIFEXITED(status)){//ie child terminated normally
        //read from shm
        for(i = 0; i < 4; ++i){
          printf("%d ", shm_ptr[i]);
        }
        printf("\n");
      }
  }


  //detach shared memory from address space
  shmdt(shm_ptr);
  //remove shared memory segment
  status = shmctl(shm_id, IPC_RMID, NULL);
  if(status < 0){
    printf("error Shared memory removal, Main\n");
  }

  return 0;
}
