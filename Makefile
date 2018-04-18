CC=gcc
CFLAGS=-Wall -lpthread
HEADER_FILES=common.h main_token_system.h shared_mem.h stats_purchases.h stats_clients.h
P=client cashier server test cleanup
LPTHREAD=lpthread

init: init.c $(HEADER_FILES) $(P)
	gcc init.c -o init -lpthread

client: client.c $(HEADER_FILES)
	gcc client.c -o client -lpthread

cashier: cashier.c $(HEADER_FILES)
	gcc cashier.c -o cashier -lpthread

server: server.c $(HEADER_FILES)
	gcc server.c -o server -lpthread

test: test.c $(HEADER_FILES)
	gcc test.c -o test -lpthread

cleanup: cleanup.c $(HEADER_FILES)
	gcc cleanup.c -o cleanup -lpthread

clean:
	rm $(P) init
