#ifndef SkillvaluesH
#define SkillvaluesH

#include <vector.h>

#include "Skillvalue.h"

// The skill file: pub/dsl001.esf onward, 900 skills to a file, kept as one list in skill id order.
struct SkillValues
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
    vector<SkillValue> records;
    int unused_value;
    String file_prefix;

    SkillValues();
    ~SkillValues();
    void ReadHeader();
    void Load();
    void SaveFilePart(int part_number, String data);
    void DeleteFiles();
    void AddSkill(int skill_id, String name, String shout, short icon, short graphic, short tp_cost, short sp_cost,
        short cast_time, short category, short spare_byte, short type, short element,
        short element_power, short target_restriction, short target_type, short target_time,
        short area, short max_level, short min_damage,
        short max_damage, short accuracy, short evade,
        short armor, short return_damage, short hp_heal,
        short tp_heal, short sp_heal, short bonus_str,
        short bonus_int, short bonus_wis, short bonus_agi,
        short bonus_con, short bonus_cha);
    void Clear();
    int FindByName(String name);
    int GetGraphic(int skill_id);
    int GetTargetRestriction(int skill_id);
    int GetTargetType(int skill_id);
    int GetCastTime(int skill_id);
    int GetTpCost(int skill_id);
    int GetSpCost(int skill_id);
    int GetType(int skill_id);
    int GetCategory(int skill_id);
    int GetIcon(int skill_id);
    String GetName(int skill_id);
    String GetShout(int skill_id);
    unsigned int Count();
    int DecodeNumber(String encoded);
};

#endif
