main: main.c common.h main_token_system.h shared_mem.h stats_purchases.h stats_clients.h client cashier
	gcc main.c -o main -lpthread

client: client.c main_token_system.h shared_mem.h
	gcc client.c -o client -lpthread

cashier: cashier.c main_token_system.h shared_mem.h common.h
	gcc cashier.c -o cashier -lpthread


clean:
	rm main client cashier
