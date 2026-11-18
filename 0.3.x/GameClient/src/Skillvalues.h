#ifndef SkillvaluesH
#define SkillvaluesH

#include <Classes.hpp>
#include <fstream>
#include <new>
#include <string>
#include <vector.h>

struct PubfileHeader;

#include "Skillvalue.h"

#define SLOT_GRID_CELL_HEIGHT 52
#define SLOT_GRID_CELL_WIDTH 45

// The skill file: pub/dsl001.esf onward, 900 skills to a file, kept as one list in skill id order.
struct SkillValues
{
    char is_downloading;
    bool download_requested;
    int expected_part_count;
    int received_part_count;
    char is_loaded;
    bool has_bad_record;
    bool header_read;
    String file_name;
    String file_path;
    int records_read;
    int records_expected;
    int checksum_high;
    int checksum_low;
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
    void ReadHeader();
    void Load();
    void SaveFilePart(int part_number, String data);
    void DeleteFiles();
    void LoadFrom(String folder);
    void Clear();
    SkillValue * FindByName(String name);
    SkillValue * GetSkill(int index);
    int Count();
    void ClearHeaders();
    void ClearFileParts();
    char ReadByte();
    int DecodeNumber(unsigned char encoded);
};

#endif
