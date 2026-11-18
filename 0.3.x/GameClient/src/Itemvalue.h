#ifndef ItemvalueH
#define ItemvalueH

enum ItemId
{
    ItemId_None = 0,
    ItemId_Gold = 1
};

enum EifItemType
{
    EifItemType_General = 1,
    EifItemType_Currency = 2,
    EifItemType_Heal = 3,
    EifItemType_Teleport = 4,
    EifItemType_Transformation = 5,
    EifItemType_ExpReward = 6,
    EifItemType_SkillBook = 7,
    EifItemType_VisualDocument = 8,
    EifItemType_Key = 9,
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
    EifItemType_CureCurse = 25,
    EifItemType_Buff = 26,
    EifItemType_Debuff = 27,
    EifItemType_TitleCertificate = 28,
    EifItemType_HairTool = 29
};

// One item of the item file.
struct ItemValue
{
    int item_id;
    String name;
    unsigned short graphic;
    unsigned char type;
    unsigned char subtype;
    unsigned char special;
    unsigned short hp;
    unsigned short tp;
    unsigned short min_damage;
    unsigned short max_damage;
    unsigned short accuracy;
    unsigned short evade;
    unsigned short armor;
    unsigned short return_damage;
    unsigned char bonus_str;
    unsigned char bonus_int;
    unsigned char bonus_wis;
    unsigned char bonus_agi;
    unsigned char bonus_con;
    unsigned char bonus_cha;
    unsigned char light_resist;
    unsigned char dark_resist;
    unsigned char earth_resist;
    unsigned char air_resist;
    unsigned char water_resist;
    unsigned char fire_resist;
    int spec_value;
    unsigned char gender;
    char spec3;
    unsigned short required_level;
    unsigned short required_class;
    unsigned short required_str;
    unsigned short required_int;
    unsigned short required_wis;
    unsigned short required_agi;
    unsigned short required_con;
    unsigned short required_cha;
    unsigned char weight;
    unsigned char size;
    unsigned char spare_byte;
    unsigned char element;
    unsigned char element_power;

    ItemValue();
    ItemValue(int record_id);
    ~ItemValue();
};

#endif
