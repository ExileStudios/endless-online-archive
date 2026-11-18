#ifndef LearndataadapterH
#define LearndataadapterH

// Keeps the skill masters in step when rows of their grid are deleted, inserted or pasted.
struct LearnDataAdapter
{
    char unused_bytes[16];

    LearnDataAdapter();
    ~LearnDataAdapter();
    int SkillLearnCount(int skill_id);
    void DeleteAt(int index);
    void InsertAt(int index);
    void CopyLists(String source_id, String target_id);
};

#endif
