#ifndef PubfileheaderH
#define PubfileheaderH

// The header of one pub file part: its length, checksum, record count and load state.
struct PubfileHeader
{
    String file_name;
    unsigned short file_length;
    unsigned short checksum_high;
    unsigned short checksum_low;
    unsigned char version;
    unsigned short record_count;
    unsigned short records_read;
    unsigned char load_state;

    PubfileHeader(String name);
    ~PubfileHeader();
};


// Shared body: a class defines its ClearHeaders with DEFINE_CLEAR_HEADERS(TheClass); each class has its own headers.
#define DEFINE_CLEAR_HEADERS(Class) \
void Class::ClearHeaders() \
{ \
    for (int i = 0; i < headers.size(); i++) \
        delete headers[i]; \
    headers.clear(); \
}

// Shared body: a class defines its ReadByte with DEFINE_READ_BYTE(TheClass); each class has its own data,
// read_position.
#define DEFINE_READ_BYTE(Class) \
char Class::ReadByte() \
{ \
    unsigned char value; \
    try \
    { \
        value = data[read_position]; \
        value--; \
    } \
    catch (...) \
    { \
        value = 0; \
    } \
    read_position++; \
    if (value > 252) \
        value = 0; \
    return value; \
}

// Shared body: a class defines its ReadNumber with DEFINE_READ_NUMBER(TheClass); each class has its own data,
// read_position.
#define DEFINE_READ_NUMBER(Class) \
int Class::ReadNumber(unsigned char length) \
{ \
    int digit; \
    int number = 0; \
    try \
    { \
        for (int i = 0; i < length; i++) \
        { \
            unsigned char byte = data[read_position + i]; \
            if (byte == 254) \
                break; \
            digit = byte; \
            digit--; \
            if (!i) \
                number += digit; \
            if (i == 1) \
                number += 253 * digit; \
            if (i == 2) \
                number += 64009 * digit; \
            if (i == 3) \
                number += digit * 0xF71AE5; \
        } \
    } \
    catch (...) \
    { \
        number = 0; \
    } \
    read_position += length; \
    return number; \
}

// Shared body: a class defines its ReadByte with DEFINE_NPC_READ_BYTE(TheClass); each class has its own data,
// read_position.
#define DEFINE_NPC_READ_BYTE(Class) \
unsigned char Class::ReadByte() \
{ \
    unsigned char value; \
    try \
    { \
        value = data[read_position]; \
        value--; \
    } \
    catch (...) \
    { \
        value = 0; \
    } \
    read_position++; \
    if (value > 252) \
        value = 0; \
    return value; \
}

// Reads the shops again from the folder given.
// Shared body: a class defines its LoadFrom with DEFINE_LOAD_FROM(TheClass); each class has its own file_prefix, Clear,
// is_loaded, has_bad_record, header_read, Load.
#define DEFINE_LOAD_FROM(Class) \
void Class::LoadFrom(String folder) \
{ \
    file_prefix = folder; \
    Clear(); \
    is_loaded = false; \
    has_bad_record = false; \
    header_read = false; \
    Load(); \
}

#endif
