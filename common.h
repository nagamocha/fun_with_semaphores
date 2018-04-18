#ifndef COMMON_H
#define COMMON_H
/*
Holds common definitions
do not alter SUCCESSFUL and FAILED values since might damage other functions
that depend on these values

However, the rest of the values can be changed. The following values each
have a specified significance



NO_OF_TOKENS - total amount of clients the restaurant can accomodate
LINEUP_MAX -total amount of clients that can lineup

CLIENTS - total amount clients that ./test will spawn for testing
CASHIERS  - total amount cashiers that ./test will spawn for testing


TOTAL_STAFF_SIZE - total amount cashiers and servers that restaurant can accomdate

order_t struct, abstracts order, ie item_id indexes into actual menu item,
token_id, client id self-explanatory

*/

#define SUCCESSFUL 0
#define FAILED -1

#define NO_OF_TOKENS 10
#define LINEUP_MAX 10

#define CLIENTS 10
#define CASHIERS 3



#define TOTAL_STAFF_SIZE 10

typedef struct{
    int item_id;
    int token_id;
    int client_id;
} order_t;

#endif
