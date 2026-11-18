#ifndef NpcdropH
#define NpcdropH

// An item an NPC drops: its least and most amount and its rate.
struct NpcDropItem
{
    int item_id;
    int min_amount;
    int max_amount;
    int rate;

    NpcDropItem(int id);
    ~NpcDropItem();
};

#endif
