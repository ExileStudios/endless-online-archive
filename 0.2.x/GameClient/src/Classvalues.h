#ifndef ClassvaluesH
#define ClassvaluesH

#include <fstream>
#include <new>
#include <string>
#include <vector.h>

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
    int checksum_high;
    int checksum_low;
    int record_count;
    vector<ClassValue> records;
    int unused_value;
    String file_prefix;

    ClassValues();
    ~ClassValues();
    void ReadHeader();
    void Load();
    void SaveFilePart(int part_number, String data);
    void DeleteFiles();
    void AddClass(int class_id, int parent_class_id, String name, int stat_group, short strength,
    short intelligence, short wisdom, short agility,
    short constitution, short charisma);
    void Clear();
    String GetName(int class_id);
    bool IsDescendantOf(int class_id, int ancestor_id);
    unsigned int Count();
    int DecodeNumber(String encoded);
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

// Reads a number of up to 4 bytes, low byte first: each byte less 1 is a digit in base 253.
// A byte of 254 ends the number.
// Shared body: a class defines its DecodeNumber with DEFINE_DECODE_NUMBER(TheClass).
#define DEFINE_DECODE_NUMBER(Class) \
int Class::DecodeNumber(String encoded) \
{ \
    int digit; \
    String text = encoded; \
    int value = 0; \
    try \
    { \
        for (int i = 1; i <= text.Length(); i++) \
        { \
            unsigned char raw_byte = text[i]; \
            if (raw_byte == 254) \
                break; \
            digit = raw_byte; \
            digit--; \
            if (i == 1) \
                value += digit; \
            if (i == 2) \
                value += NUMBER_ENCODING_BASE * digit; \
            if (i == 3) \
                value += NUMBER_ENCODING_BASE_SQUARED * digit; \
            if (i == 4) \
                value += digit * 0xF71AE5; \
        } \
    } \
    catch (...) \
    { \
        value = 0; \
    } \
    return value; \
}

#endif
