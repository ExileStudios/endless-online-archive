#ifndef SkillvaluesH
#define SkillvaluesH

#include <Classes.hpp>
#include <new>
#include <vector.h>

struct PubfileHeader;
struct SkillValue;

// The skill file: the numbered .esf parts under the pub folder, kept as one list in skill order.
struct SkillValues
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
    vector<SkillValue *> records;
    vector<PubfileHeader *> headers;
    String file_prefix;
    TStringList *file_parts;
    char keep_file_parts;

    SkillValues(String prefix, char keep_parts);
    ~SkillValues();
    void Load();
    void LoadFrom(String folder);
    void Clear();
    SkillValue * GetSkill(int index);
    int Count();
    char HeaderCount();
    PubfileHeader * GetHeader(unsigned char index);
    void ClearHeaders();
    void ClearFileParts();
    unsigned char ReadByte();
    int ReadNumber(unsigned char length);
};

#endif
