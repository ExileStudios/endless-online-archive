#ifndef NpcvaluesH
#define NpcvaluesH

#include <fstream>
#include <new>
#include <string>
#include <vector.h>

struct PubfileHeader;

#include "Npcvalue.h"

enum NpcType
{
    NpcType_Friendly = 0,
    NpcType_Passive = 1,
    NpcType_Aggressive = 2,
    NpcType_Untargetable = 3,
    NpcType_Monster4 = 4,
    NpcType_Monster5 = 5,
    NpcType_Shop = 6,
    NpcType_Inn = 7,
    NpcType_Bank = 9,
    NpcType_Barber = 10,
    NpcType_Guild = 11,
    NpcType_Priest = 12,
    NpcType_Law = 13,
    NpcType_Trainer = 14,
    NpcType_Quest = 15
};

// The NPC file: pub/dtn001.enf onward, 900 NPCs to a file, kept as one list in NPC id order.
struct NpcValues
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
    vector<NpcValue> records;
    int unused_value;
    vector<PubfileHeader *> headers;
    String file_prefix;

    NpcValues(String data_path);
    ~NpcValues();
    void ReadHeader();
    void SaveFilePart(int part_number, String data);
    void Load();
    void DeleteFiles();
    void Clear();
    NpcValue GetNpcByIndex(int index);
    int GetGraphic(int npc_id);
    String GetName(int npc_id);
    TPoint GetType(int npc_id);
    unsigned int Count();
    void ClearHeaders();
    char ReadByte();
    int DecodeNumber(unsigned char encoded);
};

#endif
