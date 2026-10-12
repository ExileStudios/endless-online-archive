#ifndef Ctrl_paperdollH
#define Ctrl_paperdollH

enum EifItemType
{
    EifItemType_Heal = 3,
    EifItemType_Teleport = 4,
    EifItemType_ExpReward = 6,
    EifItemType_Weapon = 10,
    EifItemType_Shield = 11,
    EifItemType_Armor = 12,
    EifItemType_Hat = 13,
    EifItemType_Boots = 14,
    EifItemType_Gloves = 15,
    EifItemType_Accessory = 16,
    EifItemType_Belt = 17,
    EifItemType_Necklace = 18,
    EifItemType_Ring = 19,
    EifItemType_Armlet = 20,
    EifItemType_Bracer = 21,
    EifItemType_Alcohol = 22,
    EifItemType_EffectPotion = 23,
    EifItemType_HairDye = 24,
    EifItemType_CureCurse = 25
};

enum PaperdollSlot
{
    PaperdollSlot_None = -1,
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
    String info_line_1;
    String info_line_2;
    String info_line_4;
    String info_line_5;
    String class_name;
    String info_line_7;
    String info_line_8;
    int icon_type;
    int frame_index;
    int extra_value;
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
    int type19_left_item_id;
    int type19_right_item_id;
    int belt_graphic;
    int gloves_graphic;
    int boots_graphic;
    int type19_left_graphic;
    int type19_right_graphic;
    int equipped_belt;
    int equipped_gloves;
    int equipped_boots;
    int equipped_type19_left;
    int equipped_type19_right;
    int belt_item_size;
    int gloves_item_size;
    int boots_item_size;
    int type19_left_item_size;
    int type19_right_item_size;
    int type20_left_item_id;
    int type20_right_item_id;
    int type21_left_item_id;
    int type21_right_item_id;
    int type16_item_id;
    int type20_left_graphic;
    int type20_right_graphic;
    int type21_left_graphic;
    int type21_right_graphic;
    int type16_graphic;
    int equipped_type20_left;
    int equipped_type20_right;
    int equipped_type21_left;
    int equipped_type21_right;
    int equipped_type16;
    int type20_left_item_size;
    int type20_right_item_size;
    int type21_left_item_size;
    int type21_right_item_size;
    int type16_item_size;
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
    void Open(int frame_value, String line_1_text, String line_2_text, String line_4_text, String line_5_text,
        String player_class_name, String line_7_text, String line_8_text, int new_icon_type, int unused_value,
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
    void SetType19LeftSlot(int item_id);
    void SetType19RightSlot(int item_id);
    void SetType20LeftSlot(int item_id);
    void SetType20RightSlot(int item_id);
    void SetType21LeftSlot(int item_id);
    void SetType21RightSlot(int item_id);
    void SetType16Slot(int item_id);
    bool RemoveCursedItems();
};

#endif
