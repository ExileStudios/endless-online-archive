#ifndef NpcvalueH
#define NpcvalueH

// One NPC of the NPC file.
struct NpcValue
{
    int npc_id;
    String name;
    short graphic;
    short npc_record_byte_at_3;
    short npc_record_word_at_4;
    short npc_record_word_at_6;
    short npc_type;
    short npc_record_word_at_10;
    int npc_record_triple_at_12;
    short npc_record_word_at_15;
    short npc_record_word_at_17;
    short npc_record_word_at_19;
    short npc_record_word_at_21;
    short npc_record_word_at_23;
    short npc_record_word_at_25;
    short npc_record_byte_at_27;
    short npc_record_word_at_28;
    short npc_record_word_at_30;
    short npc_record_word_at_32;
    short npc_record_word_at_34;
    short npc_record_byte_at_36;
    int npc_record_triple_at_37;

    NpcValue(int record_id);
    ~NpcValue();
};

#endif
