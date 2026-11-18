#ifndef NpcvaluesH
#define NpcvaluesH

#include <new>
#include <vector.h>

struct PubfileHeader;

#include "Npcvalue.h"

enum NpcIniFormat
{
    NpcIniFormat_Drops = 0,
    NpcIniFormat_Chats = 1,
    NpcIniFormat_ChatRates = 2
};

// The NPCs read from the drop and chat .ini files, kept in NPC id order.
struct NpcValues
{
    bool is_downloading;
    int expected_part_count;
    short received_part_count;
    char unused_flag;
    char unused_bytes[1];
    bool is_loaded;
    bool has_bad_record;
    bool header_read;
    String unused_text;
    String second_unused_text;
    short unused_word;
    short second_unused_word;
    short third_unused_word;
    short fourth_unused_word;
    int checksum_high;
    short fifth_unused_word;
    short sixth_unused_word;
    int record_count;
    bool second_unused_flag;
    bool third_unused_flag;
    char fourth_unused_flag;
    String data;
    char fifth_unused_flag;
    char sixth_unused_flag;
    vector<NpcValue> records;
    int record_index;
    vector<PubfileHeader *> headers;
    String third_unused_text;
    String file_prefix;

    NpcValues(String prefix);
    ~NpcValues();
    int ImportIni(int format, String path);
    int ParseNumber(String text);
    int ParseRate(String text);
    String KeepNumbers(String text);
    void Clear();
    NpcValue * GetNpc(int index);
    unsigned int Count();
    void ClearHeaders();
};

int CopyNpcDrops(int first, int last, int dest);
int CopyStrings(int first, int last, int dest);

// The digits of the text as a number; 0 when it has none.
// Shared body: a class defines its ParseNumber with DEFINE_PARSE_NUMBER(TheClass).
#define DEFINE_PARSE_NUMBER(Class) \
int Class::ParseNumber(String text) \
{ \
    String digits = ""; \
    for (int i = 1; i <= text.Length(); i++) \
    { \
        if (text[i] >= '0' && text[i] <= '9') \
            digits += text[i]; \
    } \
    if (digits.Length() < 1) \
        return 0; \
    return StrToInt(digits); \
}

#endif
