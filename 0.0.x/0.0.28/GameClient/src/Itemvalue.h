#ifndef ItemvalueH
#define ItemvalueH

// One item of the item file.
struct ItemValue
{
    int item_id;
    String name;
    short graphic;
    short type;
    short subtype;
    short special;
    short item_record_word_at_6;
    short item_record_word_at_8;
    short item_record_word_at_10;
    short item_record_word_at_12;
    short item_record_word_at_14;
    short item_record_word_at_16;
    short item_record_word_at_18;
    short item_record_byte_at_20;
    short item_record_byte_at_21;
    short item_record_byte_at_22;
    short item_record_byte_at_23;
    short item_record_byte_at_24;
    short item_record_byte_at_25;
    short item_record_byte_at_26;
    short item_record_byte_at_27;
    short item_record_byte_at_28;
    short item_record_byte_at_29;
    short item_record_byte_at_30;
    short item_record_byte_at_31;
    short item_record_byte_at_32;
    int spec_value;
    short gender;
    short item_record_byte_at_37;
    short required_level;
    short required_class;
    short required_str;
    short required_int;
    short required_wis;
    short required_agi;
    short required_con;
    short required_cha;
    short weight;
    short size;
    short item_record_byte_at_57;
    short item_record_byte_at_54;
    short item_record_byte_at_55;

    ItemValue(int record_id);
    ~ItemValue();
};

#endif
