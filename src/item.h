typedef struct{
    char *name; /*item name*/
    int price;  /*price of item*/
    int ID;     /*the id of the item*/
    int consumable; /*1 for true, 0 for false*/
} Item;

Item createItem(char *name, int price, int ID, int consumable);
int isConsumable(Item item);
