#ifndef ClassvaluesH
#define ClassvaluesH

#include <vector.h>
#define NUMBER_ENCODING_BASE_CUBED 16194277
#define NUMBER_ENCODING_BASE_SQUARED 64009

#include "Classvalue.h"
#define PUB_RECORDS_PER_FILE 900
#define CLASS_RECORDS_PER_FILE 250

// The class file: pub/dat001.ecf onward, 250 classes to a file, kept as one list in class id order.
struct ClassValues
{
    unsigned char is_downloading;
    unsigned char unread_flag_at_1;
    int expected_part_count;
    int received_part_count;
    char is_loaded;
    int checksum_high;
    int checksum_low;
    int record_count;
    char pad_0x1c[4];
    vector<ClassValue> records;
    int unread_int_at_64;
    String file_prefix;

    ClassValues();
    ~ClassValues();
    void ReadHeader();
    void Load();
    void SaveFilePart(int part_number, String data);
    void DeleteFiles();
    void AddClass(int class_id, int parent_class_id, String name, short class_record_byte_at_3,
        short class_record_word_at_4, short class_record_word_at_6, short class_record_word_at_8,
        short class_record_word_at_10, short class_record_word_at_12, short class_record_word_at_14);
    void Clear();
    String GetName(int class_id);
    bool IsDescendantOf(int class_id, int ancestor_id);
    unsigned int Count();
    int DecodeNumber(String encoded);
};

// Reads a number of up to 4 bytes, low byte first: each byte less 1 is a digit in base 253.
// A byte of 254 ends the number.
// Shared body: a class defines its DecodeNumber with DEFINE_DECODE_NUMBER(TheClass).
#define DEFINE_DECODE_NUMBER(Class) \
int Class::DecodeNumber(String encoded) \
{ \
    int value; \
    String text = encoded; \
    value = 0; \
    try \
    { \
        for (int i = 1; i <= text.Length(); i++) \
        { \
            unsigned char raw_byte = text[i]; \
            if (raw_byte == 254) \
                break; \
            int digit = raw_byte; \
            digit--; \
            if (i == 1) \
                value += digit; \
            if (i == 2) \
                value += NUMBER_ENCODING_BASE * digit; \
            if (i == 3) \
                value += NUMBER_ENCODING_BASE_SQUARED * digit; \
            if (i == 4) \
                value += NUMBER_ENCODING_BASE_CUBED * digit; \
        } \
    } \
    catch (...) \
    { \
        value = 0; \
    } \
    return value; \
}

// Shared body: a class defines its DecodeNumber with DEFINE_CLIENT_DECODE_NUMBER(TheClass).
#define DEFINE_CLIENT_DECODE_NUMBER(Class) \
unsigned int Class::DecodeNumber(String encoded) \
{ \
    int value = 0; \
    try \
    { \
        for (int i = 1; i <= encoded.Length(); i++) \
        { \
            unsigned char raw_byte = encoded[i]; \
            if (raw_byte == 254) \
                break; \
            int digit = raw_byte; \
            digit--; \
            if (i == 1) \
                value += digit; \
            if (i == 2) \
                value += NUMBER_ENCODING_BASE * digit; \
            if (i == 3) \
                value += NUMBER_ENCODING_BASE_SQUARED * digit; \
            if (i == 4) \
                value += NUMBER_ENCODING_BASE_CUBED * digit; \
        } \
    } \
    catch (...) \
    { \
        value = 0; \
    } \
    return value; \
}

// Empties the list.
// Shared body: a class defines its Clear with DEFINE_CLEAR(TheClass); each class has its own Count, records,
// unread_int_at_64.
#define DEFINE_CLEAR(Class) \
void Class::Clear() \
{ \
    int __slot1; \
    int __slot2; \
    if (Count() >= 1) \
    { \
        records.clear(); \
        unread_int_at_64 = -1; \
    } \
}

#endif
