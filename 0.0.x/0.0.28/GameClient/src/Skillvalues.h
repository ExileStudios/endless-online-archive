#ifndef SkillvaluesH
#define SkillvaluesH

#include <vector.h>

#include "Skillvalue.h"

// The skill file: pub/dsl001.esf onward, 900 skills to a file, kept as one list in skill id order.
struct SkillValues
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
    vector<SkillValue> records;
    int unread_int_at_64;
    String file_prefix;

    SkillValues();
    ~SkillValues();
    void ReadHeader();
    void Load();
    void SaveFilePart(int part_number, String data);
    void DeleteFiles();
    void AddSkill(int skill_id, String name, String shout, short icon, short graphic, short tp_cost, short sp_cost,
        short cast_time, short category, short skill_record_byte_at_13, short type, short skill_record_byte_at_17,
        short skill_record_word_at_18, short target_restriction, short target_type, short skill_record_byte_at_22,
        short skill_record_byte_at_23, short skill_record_word_at_24, short skill_record_word_at_26,
        short skill_record_word_at_28, short skill_record_word_at_30, short skill_record_word_at_32,
        short skill_record_word_at_34, short skill_record_byte_at_36, short skill_record_word_at_37,
        short skill_record_word_at_39, short skill_record_byte_at_41, short skill_record_word_at_42,
        short skill_record_word_at_44, short skill_record_word_at_46, short skill_record_word_at_48,
        short skill_record_word_at_50, short skill_record_word_at_52);
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
