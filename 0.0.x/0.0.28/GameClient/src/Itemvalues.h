#ifndef ItemvaluesH
#define ItemvaluesH

#include <vector.h>

#include "Itemvalue.h"

// The item file: pub/dat001.eif onward, 900 items to a file, kept as one list in item id order.
struct ItemValues
{
    char is_downloading;
    unsigned char unread_flag_at_1;
    int expected_part_count;
    int received_part_count;
    char is_loaded;
    int checksum_high;
    int checksum_low;
    int record_count;
    char pad_0x1c[4];
    vector<ItemValue> records;
    int unread_int_at_64;
    String file_prefix;

    ItemValues();
    ~ItemValues();
    void ReadHeader();
    void SaveFilePart(int part_number, String data);
    void Load();
    void DeleteFiles();
    void AddItem(int item_id, String name, short graphic, short type, short subtype, short special,
        short item_record_word_at_6, short item_record_word_at_8, short item_record_word_at_10,
        short item_record_word_at_12, short item_record_word_at_14, short item_record_word_at_16,
        short item_record_word_at_18, short item_record_byte_at_20, short item_record_byte_at_21,
        short item_record_byte_at_22, short item_record_byte_at_23, short item_record_byte_at_24,
        short item_record_byte_at_25, short item_record_byte_at_26, short item_record_byte_at_27,
        short item_record_byte_at_28, short item_record_byte_at_29, short item_record_byte_at_30,
        short item_record_byte_at_31, short item_record_byte_at_32, int spec_value, short gender,
        short item_record_byte_at_37, short required_level, short required_class, short required_str,
        short required_int, short required_wis, short required_agi, short required_con, short required_cha,
        short item_record_byte_at_54, short item_record_byte_at_55, short weight, short item_record_byte_at_57,
        short size);
    void Clear();
    ItemValue GetItem(int item_id);
    int GetWeight(int item_id);
    int GetGender(int item_id);
    int GetGraphic(int item_id);
    int GetSize(int item_id);
    int GetSpecial(int item_id);
    int GetType(int item_id);
    int GetSubType(int item_id);
    int GetClassRequirement(int item_id);
    int GetSpecValue(int item_id);
    String GetKeyName(int key_id);
    String GetName(int item_id);
    unsigned int Count();
    int DecodeNumber(String encoded);
};

#endif
