#ifndef NpcvaluesH
#define NpcvaluesH

#include <vector.h>

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
struct NpcTypeInfo
{
    int npc_type;
    int behavior_id;
};

// The NPC file: pub/dtn001.enf onward, 900 NPCs to a file, kept as one list in NPC id order.
struct NpcValues
{
    char is_downloading;
    unsigned char download_requested;
    int expected_part_count;
    int received_part_count;
    char is_loaded;
    int checksum_high;
    int checksum_low;
    int record_count;
    char unused_bytes[4];
    vector<NpcValue> records;
    int unused_value;
    String file_prefix;

    NpcValues();
    ~NpcValues();
    void ReadHeader();
    void SaveFilePart(int part_number, String data);
    void Load();
    void DeleteFiles();
    void AddNpc(int npc_id, String name, short graphic, short race, short boss,
        short child, short npc_type, short behavior_id, int hp,
        short tp, short min_damage, short max_damage,
        short accuracy, short evade, short armor,
        short return_damage, short element, short element_damage,
        short element_weakness, short element_weakness_damage, short level,
        int experience);
    void Clear();
    int GetGraphic(int npc_id);
    String GetName(int npc_id);
    NpcTypeInfo GetType(int npc_id);
    unsigned int Count();
    int DecodeNumber(String encoded);
};

#endif
