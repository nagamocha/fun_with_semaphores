main: main_with_sem.c client sem_queue
	gcc main_with_sem.c -o main -lpthread

client: client.c
	gcc client.c -o client -lpthread


sem_queue: sem_queue.c
	gcc sem_queue.c -o sem_queue -lpthread
