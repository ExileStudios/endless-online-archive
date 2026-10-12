#ifndef SkillvalueH
#define SkillvalueH

// One skill of the skill file.
struct SkillValue
{
    int skill_id;
    String name;
    String shout;
    short skill_record_word_at_24;
    short icon;
    short graphic;
    short tp_cost;
    short sp_cost;
    short cast_time;
    short category;
    short skill_record_byte_at_13;
    int type;
    short skill_record_byte_at_17;
    short skill_record_word_at_18;
    short target_restriction;
    short target_type;
    short skill_record_byte_at_22;
    short skill_record_byte_at_23;
    short skill_record_word_at_26;
    short skill_record_word_at_28;
    short skill_record_word_at_30;
    short skill_record_word_at_32;
    short skill_record_word_at_34;
    short skill_record_byte_at_36;
    short skill_record_word_at_37;
    short skill_record_word_at_39;
    short skill_record_byte_at_41;
    short skill_record_word_at_42;
    short skill_record_word_at_44;
    short skill_record_word_at_46;
    short skill_record_word_at_48;
    short skill_record_word_at_50;
    short skill_record_word_at_52;

    SkillValue(int record_id);
    ~SkillValue();
};

#endif
