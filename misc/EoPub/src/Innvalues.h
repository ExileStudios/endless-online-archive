#ifndef InnvaluesH
#define InnvaluesH

#include <new>
#include <vector.h>

struct InnValue;
struct PubfileHeader;

// A question an innkeeper asks and its answer.
struct InnQuizItem
{
    String question;
    String answer;

    InnQuizItem();
    ~InnQuizItem();
};

// The inns read from serv_inns.epf in the pub folder.
struct InnValues
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
    vector<InnValue *> records;
    vector<PubfileHeader *> headers;
    String file_prefix;

    InnValues(String prefix);
    ~InnValues();
    void Load();
    void LoadFrom(String folder);
    void Clear();
    InnValue * GetInn(int index);
    int Count();
    char HeaderCount();
    PubfileHeader * GetHeader(unsigned char index);
    void ClearHeaders();
    void InsertInn(int index);
    void DeleteInn(int index);
    char ReadByte();
    int ReadNumber(unsigned char length);
};

// One inn: its sleep cost, where it puts players to sleep and back to life, and its questions.
struct InnValue
{
    unsigned short id;
    unsigned short behavior_id;
    String name;
    int sleep_cost;
    unsigned short sleep_map;
    unsigned char sleep_x;
    unsigned char sleep_y;
    unsigned short spawn_map;
    unsigned char spawn_x;
    unsigned char spawn_y;
    unsigned short alternate_spawn_level;
    unsigned short alternate_spawn_map;
    unsigned char alternate_spawn_x;
    unsigned char alternate_spawn_y;
    vector<InnQuizItem> questions;

    InnValue();
    InnValue(int record_id);
    ~InnValue();
    void AddQuestion(String question, String answer);
};

#endif
