#ifndef PubfileheaderH
#define PubfileheaderH

struct PubfileHeader
{
    String file_name;
    short file_length;
    short checksum_high;
    short checksum_low;
    char version;
    unsigned short record_count;
    short records_read;
    unsigned char load_state;

    PubfileHeader(String name);
    ~PubfileHeader();
};


// Deletes every item record.
// Shared body: a class defines its ClearHeaders with DEFINE_CLEAR_HEADERS(TheClass); each class has its own headers.
#define DEFINE_CLEAR_HEADERS(Class) \
void Class::ClearHeaders() \
{ \
    for (int i = 0; i < headers.size(); i++) \
        delete headers[i]; \
    headers.clear(); \
}

// Reads one byte at the read position, stored plus one; a value past 252 reads as 0.
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

#endif
