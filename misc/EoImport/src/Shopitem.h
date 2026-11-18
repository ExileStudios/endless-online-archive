#ifndef ShopitemH
#define ShopitemH

// An item a shop trades, with its prices.
struct ShopItemVal
{
    int item_id;
    int sell_price;
    int buy_price;
    int max_amount;

    ShopItemVal();
    ShopItemVal(int id);
    ~ShopItemVal();
};

#endif
