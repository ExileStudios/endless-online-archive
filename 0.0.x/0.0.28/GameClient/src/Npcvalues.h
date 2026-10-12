#ifndef NpcvaluesH
#define NpcvaluesH

#include <vector.h>

#include "Npcvalue.h"

enum NpcType
{
    NpcType_Friendly = 0,
    NpcType_Unknown1 = 1,
    NpcType_Unknown2 = 2,
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
    int npc_record_word_at_10;
};

// The NPC file: pub/dtn001.enf onward, 900 NPCs to a file, kept as one list in NPC id order.
struct NpcValues
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
    vector<NpcValue> records;
    int unread_int_at_64;
    String file_prefix;

    NpcValues();
    ~NpcValues();
    void ReadHeader();
    void SaveFilePart(int part_number, String data);
    void Load();
    void DeleteFiles();
    void AddNpc(int npc_id, String name, short graphic, short npc_record_byte_at_3, short npc_record_word_at_4,
        short npc_record_word_at_6, short npc_type, short npc_record_word_at_10, int npc_record_triple_at_12,
        short npc_record_word_at_15, short npc_record_word_at_17, short npc_record_word_at_19,
        short npc_record_word_at_21, short npc_record_word_at_23, short npc_record_word_at_25,
        short npc_record_byte_at_27, short npc_record_word_at_28, short npc_record_word_at_30,
        short npc_record_word_at_32, short npc_record_word_at_34, short npc_record_byte_at_36,
        int npc_record_triple_at_37);
    void Clear();
    int GetGraphic(int npc_id);
    String GetName(int npc_id);
    NpcTypeInfo GetType(int npc_id);
    unsigned int Count();
    int DecodeNumber(String encoded);
};

#endif
