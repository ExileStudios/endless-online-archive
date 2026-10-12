#ifndef ItemshopH
#define ItemshopH

// One item a shop buys or sells, with its price and amount.
struct ShopItem
{
    int item_id;
    int price;
    int max_amount;
    bool unused_flag_at_12;

    ShopItem(int id);
    ~ShopItem();
};

#endif
