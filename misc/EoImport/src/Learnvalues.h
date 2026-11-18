#ifndef LearnvaluesH
#define LearnvaluesH

#include <new>
#include <vector.h>

struct PubfileHeader;

#include "Learnvalue.h"

// The skill masters read from skills.ini.
struct LearnValues
{
    String value_text;
    bool is_loaded;
    bool has_bad_record;
    bool header_read;
    String file_name;
    String file_path;
    int records_read;
    short record_count_low;
    short record_count_high;
    String data;
    short read_position_low;
    short read_position_high;
    vector<LearnValue> records;
    int record_index;
    vector<PubfileHeader *> headers;
    String file_prefix;

    LearnValues(String prefix);
    ~LearnValues();
    int ImportIni(int format, String path);
    int ReadNextNumber();
    int ParseNumber(String text);
    LearnValue * GetTrainer(int index);
    int Count();
};

int CopyLearnItems(int first, int last, int dest);

// Takes the next number of a comma list off the value text; -1 when none is left.
// Shared body: a class defines its ReadNextNumber with DEFINE_READ_NEXT_NUMBER(TheClass); each class has its own
// value_text, ParseNumber.
#define DEFINE_READ_NEXT_NUMBER(Class) \
int Class::ReadNextNumber() \
{ \
    int i; \
    int length = value_text.Length(); \
    for (i = 1; i <= length; i++) \
    { \
        if (value_text[i] == ',' || i == value_text.Length()) \
        { \
            int number = ParseNumber(value_text.SubString(1, i)); \
            value_text.Delete(1, i); \
            return number; \
        } \
    } \
    return -1; \
}

#endif
