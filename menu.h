#ifndef MENU_H
#define MENU_H
#include <string.h>
//there will be 20 menu items hardcoded
//item id implicit, index in menu array
#define TOTAL_MENU_ITEMS 20

typedef struct{
    char name[24];
    float price;
    int time_min;
    int time_max;
} menu_item_t;


void menu_item_set(menu_item_t *mn, char* n, float p, int min, int max){
    mn->price = p;
    strcpy(mn->name, n);
    mn->time_min = min;
    mn->time_max = max;
}

//zero index
//assumes menu has 20 slots or more
void menu_init(menu_item_t *mn_arr){
    int i;
    menu_item_set(mn_arr +  0, "BBQ-Chicken-Salad"   ,  8.95, 18, 24);
    menu_item_set(mn_arr +  1, "Spinach Power"       ,  9.15, 12, 16);
    menu_item_set(mn_arr +  2, "Garden-Salad"        ,  4.75, 10, 13);
    menu_item_set(mn_arr +  3, "Steak-Blue-Cheese"   ,  7.25, 12, 15);
    menu_item_set(mn_arr +  4, "Caesars-Salad"       ,  6.75, 13, 15);
    menu_item_set(mn_arr +  5, "Chicken-Salad"       ,  9.15, 15, 21);
    menu_item_set(mn_arr +  6, "Mongolian-BBQ-Plate" ,  9.75, 21, 31);
    menu_item_set(mn_arr +  7, "Club-Sandwich"       ,  6.35, 13, 18);
    menu_item_set(mn_arr +  8, "Belgian-Cheese-Sub"  , 10.25, 15, 19);
    menu_item_set(mn_arr +  9, "Rio-Grande-beef-Sub" ,  9.35, 18, 20);
    menu_item_set(mn_arr + 10, "Argentine-Asado-Club", 11.75, 23, 30);
    menu_item_set(mn_arr + 11, "Sierra-Sub"          , 10.38, 12, 15);
    menu_item_set(mn_arr + 12, "Avocado-BLT"         ,  8.05, 12, 13);
    menu_item_set(mn_arr + 13, "Soup-de-Egion"       ,  3.20, 11, 15);
    menu_item_set(mn_arr + 14, "Soup-de-Sur"         ,  2.75,  6,  9);
    menu_item_set(mn_arr + 15, "Coffee"              ,  1.25,  2,  4);
    menu_item_set(mn_arr + 16, "Hot-Tea"             ,  1.05,  1,  4);
    menu_item_set(mn_arr + 17, "Hot-Chocolate"       ,  2.15,  1,  2);
    menu_item_set(mn_arr + 18, "Mocha"               ,  3.25,  2,  3);
    menu_item_set(mn_arr + 19, "Cafe-Latte"          ,  3.75,  5,  7);
}



float menu_get_value(menu_item_t *mn, int item_id){
    return (mn[item_id-1]).price;
}

char* menu_get_name(menu_item_t *mn, int item_id){
    return (mn[item_id-1]).name;
}

int menu_get_time_max(menu_item_t *mn, int item_id){
    return (mn[item_id-1]).time_max;
}

int menu_get_time_min(menu_item_t *mn, int item_id){
    return (mn[item_id-1]).time_min;
}




















#endif












//end
