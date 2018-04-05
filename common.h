#include <semaphore.h>

typedef struct{
  sem_t sem;
  int arr[4];
} s_memory_cw;
