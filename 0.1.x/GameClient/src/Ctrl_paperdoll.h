#ifndef Ctrl_paperdollH
#define Ctrl_paperdollH


enum PaperdollSlot
{
    PaperdollSlot_None = -1,
    PaperdollSlot_NoSlot = 0,
    PaperdollSlot_Head = 1,
    PaperdollSlot_Necklace = 2,
    PaperdollSlot_Weapon = 3,
    PaperdollSlot_Armor = 4,
    PaperdollSlot_Shield = 5,
    PaperdollSlot_Gloves = 6,
    PaperdollSlot_Belt = 7,
    PaperdollSlot_Text71Left = 8,
    PaperdollSlot_Text71Right = 9,
    PaperdollSlot_Boots = 10,
    PaperdollSlot_Text72Left = 11,
    PaperdollSlot_Text72Right = 12,
    PaperdollSlot_Text73Left = 13,
    PaperdollSlot_Text73Right = 14,
    PaperdollSlot_Text76 = 15
};

enum ItemSpecial
{
    ItemSpecial_Bound = 4,
    ItemSpecial_Cursed = 5
};

// The paperdoll window: a character's equipment boxes and info lines, and the own character's equipment.
// The *_item_id and *_graphic members are the boxes shown, the equipped_* members the own character's equipment.
struct PaperdollController
{
    char close_hovered;
    char is_visible;
    int window_x;
    int window_y;
    String player_name;
    String home_name;
    String partner_name;
    String title;
    String class_name;
    String guild_name;
    String guild_rank_name;
    int icon_type;
    int frame_index;
    int admin_level;
    int head_item_id;
    int necklace_item_id;
    int armor_item_id;
    int weapon_item_id;
    int shield_item_id;
    int head_graphic;
    int necklace_graphic;
    int armor_graphic;
    int weapon_graphic;
    int shield_graphic;
    int equipped_head;
    int equipped_necklace;
    int equipped_armor;
    int equipped_weapon;
    int equipped_shield;
    int head_item_size;
    int necklace_item_size;
    int armor_item_size;
    int weapon_item_size;
    int shield_item_size;
    int belt_item_id;
    int gloves_item_id;
    int boots_item_id;
    int ring1_item_id;
    int ring2_item_id;
    int belt_graphic;
    int gloves_graphic;
    int boots_graphic;
    int ring1_graphic;
    int ring2_graphic;
    int equipped_belt;
    int equipped_gloves;
    int equipped_boots;
    int equipped_ring1;
    int equipped_ring2;
    int belt_item_size;
    int gloves_item_size;
    int boots_item_size;
    int ring1_item_size;
    int ring2_item_size;
    int armlet1_item_id;
    int armlet2_item_id;
    int bracer1_item_id;
    int bracer2_item_id;
    int accessory_item_id;
    int armlet1_graphic;
    int armlet2_graphic;
    int bracer1_graphic;
    int bracer2_graphic;
    int accessory_graphic;
    int equipped_armlet1;
    int equipped_armlet2;
    int equipped_bracer1;
    int equipped_bracer2;
    int equipped_accessory;
    int armlet1_item_size;
    int armlet2_item_size;
    int bracer1_item_size;
    int bracer2_item_size;
    int accessory_item_size;
    char is_own_character;
    int drag_mode;
    int hovered_slot;
    char is_dragging;
    int drag_offset_x;
    int drag_offset_y;
    char unused_flag;

    PaperdollController();
    ~PaperdollController();
    void EquipDroppedItem(int item_id, int x, int y);
    void Open(int frame_row, String name_text, String home_text, String partner_text, String title_text,
        String player_class_name, String guild_text, String guild_rank_text, int new_icon_type, int new_admin_level,
        char own_character_flag);
    void MouseDown(int x, int y, int button);
    void MouseMove(int x, int y);
    void MouseUp();
    void SetHeadSlot(int item_id);
    void SetNecklaceSlot(int item_id);
    void SetArmorSlot(int item_id);
    void SetWeaponSlot(int item_id);
    void SetShieldSlot(int item_id);
    void SetBeltSlot(int item_id);
    void SetGlovesSlot(int item_id);
    void SetBootsSlot(int item_id);
    void SetRing1Slot(int item_id);
    void SetRing2Slot(int item_id);
    void SetArmlet1Slot(int item_id);
    void SetArmlet2Slot(int item_id);
    void SetBracer1Slot(int item_id);
    void SetBracer2Slot(int item_id);
    void SetAccessorySlot(int item_id);
    bool RemoveCursedItems();
};

#endif
