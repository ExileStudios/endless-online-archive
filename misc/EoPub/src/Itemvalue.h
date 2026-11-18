#ifndef ItemvalueH
#define ItemvalueH

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
    unsigned char spec3;
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
