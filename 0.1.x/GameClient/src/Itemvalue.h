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
    EifItemType_Spell = 5,
    EifItemType_ExpReward = 6,
    EifItemType_StatReward = 7,
    EifItemType_SkillReward = 8,
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
    short graphic;
    short type;
    short subtype;
    short special;
    unsigned short hp;
    unsigned short tp;
    int min_damage;
    int max_damage;
    int accuracy;
    int evade;
    int armor;
    int return_damage;
    unsigned short bonus_str;
    unsigned short bonus_int;
    unsigned short bonus_wis;
    unsigned short bonus_agi;
    unsigned short bonus_con;
    unsigned short bonus_cha;
    short light_resist;
    short dark_resist;
    short earth_resist;
    short air_resist;
    short water_resist;
    short fire_resist;
    int spec_value;
    unsigned short gender;
    short spec3;
    short required_level;
    short required_class;
    unsigned short required_str;
    unsigned short required_int;
    unsigned short required_wis;
    unsigned short required_agi;
    unsigned short required_con;
    unsigned short required_cha;
    short weight;
    short size;
    short spare_byte;
    short element;
    short element_power;

    ItemValue(int record_id);
    ~ItemValue();
};

#endif
