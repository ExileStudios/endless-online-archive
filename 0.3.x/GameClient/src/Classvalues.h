#ifndef ClassvaluesH
#define ClassvaluesH

#include <Classes.hpp>
#include <fstream>
#include <new>
#include <string>
#include <vector.h>

struct PubfileHeader;

#include "Classvalue.h"

#define CLASS_RECORDS_PER_FILE 250
#define NUMBER_ENCODING_BASE_CUBED 16194277
#define NUMBER_ENCODING_BASE_SQUARED 64009

// The class file: pub/dat001.ecf onward, 250 classes to a file, kept as one list in class id order.
struct ClassValues
{
    unsigned char is_downloading;
    bool download_requested;
    int expected_part_count;
    int received_part_count;
    char is_loaded;
    bool has_long_name;
    bool file_missing;
    String part_file_name;
    String part_file_path;
    int records_read;
    int record_count;
    int checksum_high;
    int checksum_low;
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
    void ReadHeader();
    void Load();
    void SaveFilePart(int part_number, String data);
    void DeleteFiles();
    void LoadFrom(String folder);
    void Clear();
    ClassValue * GetRecord(int index);
    bool IsDescendantOf(int class_id, int ancestor_id);
    int Count();
    void ClearHeaders();
    void ClearKeptFiles();
    char ReadByte();
    int DecodeNumber(unsigned char encoded);
};


// Shared body: a class defines its DecodeNumber with DEFINE_DECODE_NUMBER(TheClass); each class has its own data,
// read_position.
#define DEFINE_DECODE_NUMBER(Class) \
int Class::DecodeNumber(unsigned char encoded) \
{ \
    int digit; \
    int value = 0; \
    try \
    { \
        for (int i = 0; i < encoded; i++) \
        { \
            unsigned char raw_byte = data[read_position + i]; \
            if (raw_byte == 254) \
                break; \
            digit = raw_byte; \
            digit--; \
            if (!i) \
                value += digit; \
            if (i == 1) \
                value += 253 * digit; \
            if (i == 2) \
                value += 64009 * digit; \
            if (i == 3) \
                value += digit * 0xF71AE5; \
        } \
    } \
    catch (...) \
    { \
        value = 0; \
    } \
    read_position += encoded; \
    return value; \
}

#endif
