#ifndef InnvaluesH
#define InnvaluesH

#include <new>
#include <vector.h>

struct PubfileHeader;

// A question an innkeeper asks and its answer.
struct InnQuizItem
{
    String question;
    String answer;

    InnQuizItem();
    ~InnQuizItem();
};

// One inn: its sleep cost, where it puts players to sleep and back to life, and its questions.
struct InnValue
{
    short id;
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

    InnValue(int record_id);
    ~InnValue();
};

// The inns read from inn.ini or home.ini.
struct InnValues
{
    String value_text;
    bool is_loaded;
    bool has_bad_record;
    bool header_read;
    String file_name;
    String file_path;
    int records_read;
    int record_count;
    String data;
    int read_position;
    vector<InnValue> records;
    int record_index;
    vector<PubfileHeader *> headers;
    String file_prefix;

    InnValues(String prefix);
    ~InnValues();
    int ImportIni(int format, String path);
    int ReadNextNumber();
    int ParseNumber(String text);
    InnValue * GetInn(int index);
    int Count();
};

int CopyInnQuizItems(int first, int last, int dest);

#endif
