#ifndef ClassvaluesH
#define ClassvaluesH

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
    vector<ClassValue> records;
    int unused_value;
    vector<PubfileHeader *> part_headers;
    String file_prefix;

    ClassValues(String path);
    ~ClassValues();
    void ReadHeader();
    void Load();
    void SaveFilePart(int part_number, String data);
    void DeleteFiles();
    void LoadFrom(String folder);
    void Clear();
    String GetName(int class_id);
    bool IsDescendantOf(int class_id, int ancestor_id);
    unsigned int Count();
    void ClearHeaders();
    char ReadByte();
    int DecodeNumber(unsigned char encoded);
};


// Empties the list.
// Shared body: a class defines its Clear with DEFINE_CLEAR(TheClass); each class has its own Count, records,
// unused_value.
#define DEFINE_CLEAR(Class) \
void Class::Clear() \
{ \
    if (Count() >= 1) \
    { \
        records.clear(); \
        unused_value = -1; \
    } \
}

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
