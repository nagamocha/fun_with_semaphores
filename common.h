#ifndef COMMON_H
#define COMMON_H


#define SUCCESSFUL 0
#define FAILED -1

#define NO_OF_TOKENS 3
#define LINEUP_MAX 3

#define CLIENTS 10
#define CASHIERS 3

#define BQ_MAX 3
#define BQ_MAX_WAITING_TIME 5

typedef struct{
    int item_id;
    int token_id;
    int client_id;
} order_t;

#endif
