#ifndef NpcvaluesH
#define NpcvaluesH

#include <Classes.hpp>
#include <SysUtils.hpp>
#include <ScktComp.hpp>
#include <DBTables.hpp>
#include <string.h>
#include <stdio.h>
#include <vector.h>
#include <deque.h>
#include <list.h>
#include <stack.h>
#include <queue.h>

enum NpcType
{
    NpcType_Invalid = -1,
    NpcType_Friendly = 0,
    NpcType_Shop = 6,
    NpcType_Inn = 7,
    NpcType_Priest = 12,
    NpcType_Trainer = 14,
    NpcType_Quest = 15
};

struct NpcValue;
struct MapCoord;

#include "Npcvalue.h"

struct NpcValues;

// The npcs, read from the pub/dtn*.enf files, with their drops ( pub/dtd001.edf ) and talk ( pub/ttd001.etf ).
struct NpcValues
{
    int file_id;
    int num_records;
    int rid[2];
    char loaded;
    bool drops_loaded;
    bool talk_loaded;
    TStringList *string_list;
    void *encode_scratch;
    vector<NpcValue> record_list;
    int record_index;

    NpcValues();
    ~NpcValues();
    void LoadNpcs();
    void LoadDrops();
    void LoadTalk();
    void AddDrop(int npc_id, int item_id, int min_amount, int max_amount, int rate);
    void SetTalk(int npc_id, int rate, String message);
    String RollTalk(int enf_id);
    MapCoord GetDrop(int npc_id);
    void AddNpc(int id, String name, short graphic_id, short race, short boss, short child, short npc_type,
        short behavior_id, int hp, short tp, short min_damage, short max_damage, short accuracy, short evade,
        short armor, short return_damage, short element, short element_damage, short element_weakness,
        short element_weakness_damage, short level, int experience);
    NpcValue GetNpc(int id);
    int GetExp(int npc_id);
    int GetMaxHp(int npc_id);
    MapCoord GetType(int enf_id);
    int GetCount();
    int DecodeNumber(String value);
};


#endif
