#ifndef LearnvaluesH
#define LearnvaluesH

#include <new>
#include <vector.h>

struct LearnValue;
struct PubfileHeader;

// The skill masters read from serv_trainers.epf in the pub folder.
struct LearnValues
{
    bool is_loaded;
    bool has_bad_record;
    bool header_read;
    String file_name;
    String file_path;
    int records_read;
    int record_count;
    String data;
    int read_position;
    vector<LearnValue *> records;
    vector<PubfileHeader *> headers;
    String file_prefix;

    LearnValues(String prefix);
    ~LearnValues();
    void Load();
    void LoadFrom(String folder);
    void Clear();
    LearnValue * Insert(int index);
    int Count();
    char HeaderCount();
    PubfileHeader * GetHeader(unsigned char index);
    void ClearHeaders();
    void InsertTrainer(int index);
    void DeleteTrainer(int index);
    char ReadByte();
    int ReadNumber(unsigned char length);
};

#endif
