#ifndef ItemvaluesH
#define ItemvaluesH

#include <fstream>
#include <new>
#include <string>
#include <vector.h>

#include "Itemvalue.h"

#define PUB_RECORDS_PER_FILE 900

// The item file: pub/dat001.eif onward, 900 items to a file, kept as one list in item id order.
struct ItemValues
{
    char is_downloading;
    bool download_requested;
    int expected_part_count;
    int received_part_count;
    char is_loaded;
    int checksum_high;
    int checksum_low;
    int record_count;
    vector<ItemValue> records;
    int unused_value;
    String file_prefix;

    ItemValues();
    ~ItemValues();
    void ReadHeader();
    void SaveFilePart(int part_number, String data);
    void Load();
    void DeleteFiles();
    void AddItem(int item_id, String name, short graphic, short type, short subtype, short special,
    short hp, short tp, int min_damage, int max_damage,
    int accuracy, int evade, int armor, int return_damage,
    short bonus_str, short bonus_int, short bonus_wis,
    short bonus_agi, short bonus_con, short bonus_cha,
    short light_resist, short dark_resist, short earth_resist,
    short air_resist, short water_resist, short fire_resist, int spec_value,
    short gender, short spec3, short required_level, short required_class, short required_str,
    short required_int, short required_wis, short required_agi, short required_con, short required_cha,
    short element, short element_power, short weight, short spare_byte, short size);
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
