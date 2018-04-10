main: main.c common.h token.h shared_mem.h client
	gcc main.c -o main -lpthread

client: client.c token.h shared_mem.h
	gcc client.c -o client -lpthread
