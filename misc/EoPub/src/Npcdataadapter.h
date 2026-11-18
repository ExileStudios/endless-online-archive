#ifndef NpcdataadapterH
#define NpcdataadapterH

// Keeps the NPCs in step with the NPC grid's edits, and counts their drops and chat lines.
struct NpcDataAdapter
{
    int drop_npc_count;
    int drop_count;
    int chat_npc_count;
    int message_count;

    NpcDataAdapter();
    ~NpcDataAdapter();
    int CountLists();
    int ItemDropCount(int item_id);
    void DeleteAt(int index);
    void InsertAt(int index);
    void CopyLists(String source_id, String target_id);
};

#endif
