#ifndef ClassvaluesH
#define ClassvaluesH

#include <Classes.hpp>
#include <new>
#include <vector.h>

struct ClassValue;
struct PubfileHeader;

// The class file: the numbered .ecf parts under the pub folder, kept as one list in class order.
struct ClassValues
{
    char unused_bytes[12];
    bool is_loaded;
    bool has_long_name;
    bool file_missing;
    String part_file_name;
    String part_file_path;
    int records_read;
    int record_count;
    int checksum_high;
    char second_unused_bytes[4];
    int header_record_count;
    String file_text;
    int read_position;
    vector<ClassValue *> records;
    vector<PubfileHeader *> part_headers;
    String file_prefix;
    TStringList *kept_files;
    char keep_files;

    ClassValues(String folder, char keep);
    ~ClassValues();
    void Load();
    void LoadFrom(String folder);
    void Clear();
    ClassValue * GetRecord(int index);
    int Count();
    char HeaderCount();
    PubfileHeader * GetHeader(unsigned char index);
    void ClearHeaders();
    void ClearKeptFiles();
    char ReadByte();
    int ReadNumber(unsigned char length);
};

#endif
