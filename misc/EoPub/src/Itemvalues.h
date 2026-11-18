#ifndef ItemvaluesH
#define ItemvaluesH

#include <Classes.hpp>
#include <new>
#include <vector.h>

struct ItemValue;
struct PubfileHeader;

// The item file: the numbered .eif parts under the pub folder, kept as one list in item order.
struct ItemValues
{
    bool is_downloading;
    char unused_bytes[11];
    bool is_loaded;
    bool has_bad_record;
    bool header_read;
    String file_name;
    String file_path;
    int records_read;
    int records_expected;
    int checksum_high;
    char second_unused_bytes[4];
    int record_count;
    String data;
    int read_position;
    vector<ItemValue *> records;
    vector<PubfileHeader *> headers;
    String file_prefix;
    TStringList *file_parts;
    char keep_file_parts;

    ItemValues(String prefix, char keep_parts);
    ~ItemValues();
    void Load();
    void LoadFrom(String folder);
    void Clear();
    ItemValue * GetItem(int index);
    int Count();
    char HeaderCount();
    PubfileHeader * GetHeader(unsigned char index);
    void ClearHeaders();
    void ClearFileParts();
    char ReadByte();
    int ReadNumber(unsigned char length);
};

#endif
