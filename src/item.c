#include "item.h"

/*initialze an item struct*/
Item createItem(char *name, int price, int ID, int consumable){
    Item item;
    item.name = name;
    item.price = price;
    item.ID = ID;
    return item;
}

/*is the item consumable*/
int isConsumable(Item item){
    if(item.consumable){
        return 1;
    }
    return 0;
}
