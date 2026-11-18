#ifndef ItemvaluesH
#define ItemvaluesH

#include <fstream>
#include <new>
#include <string>
#include <vector.h>

struct PubfileHeader;

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
    bool has_bad_record;
    bool header_read;
    String file_name;
    String file_path;
    int records_read;
    int records_expected;
    int checksum_high;
    int checksum_low;
    int record_count;
    String data;
    int read_position;
    vector<ItemValue> records;
    int unused_value;
    vector<PubfileHeader *> headers;
    String file_prefix;

    ItemValues(String prefix);
    ~ItemValues();
    void ReadHeader();
    void SaveFilePart(int part_number, String data);
    void Load();
    void DeleteFiles();
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
    void ClearHeaders();
    unsigned char ReadByte();
    int DecodeNumber(unsigned char encoded);
};

#endif
