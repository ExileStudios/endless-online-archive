#ifndef ItemshopH
#define ItemshopH

#include <typeinfo>

// One item a shop buys or sells, with its price and amount.
struct ShopItem
{
    int item_id;
    int price;
    int max_amount;
    char unused_flag;

    ShopItem(int id);
    ~ShopItem();
};

struct type_info_hash : public std::type_info
{
    char hash_state[12];
};

#endif
