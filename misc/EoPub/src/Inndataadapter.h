#ifndef InndataadapterH
#define InndataadapterH

// Keeps the inns in step when rows of the inns grid are deleted, inserted or pasted.
struct InnDataAdapter
{
    char unused_bytes[8];

    InnDataAdapter();
    ~InnDataAdapter();
    void DeleteAt(int index);
    void InsertAt(int index);
    void CopyLists(String source_id, String target_id);
};

#endif
