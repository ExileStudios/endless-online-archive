#ifndef NpcvaluesH
#define NpcvaluesH

#include <Classes.hpp>
#include <new>
#include <vector.h>

struct NpcValue;
struct PubfileHeader;

// The NPC file: the numbered .enf parts, with the drops of serv_drops and the lines of serv_chats.
struct NpcValues
{
    bool is_downloading;
    char unused_bytes[11];
    bool is_loaded;
    bool has_bad_record;
    bool header_read;
    bool has_bad_drop;
    bool has_bad_message;
    String file_name;
    String drops_path;
    String chats_path;
    String file_path;
    int records_read;
    int records_expected;
    int checksum_high;
    char second_unused_bytes[4];
    int record_count;
    bool drops_loaded;
    int drops_read;
    bool chats_loaded;
    int messages_read;
    String data;
    int read_position;
    vector<NpcValue *> records;
    char third_unused_bytes[4];
    vector<PubfileHeader *> headers;
    String pub_folder;
    String file_prefix;
    TStringList *file_parts;
    char keep_file_parts;

    NpcValues(String prefix, char keep_parts);
    ~NpcValues();
    void Load();
    void LoadDrops();
    void LoadChats();
    void LoadFrom(String folder);
    void Clear();
    NpcValue * GetNpc(int index);
    int Count();
    char HeaderCount();
    PubfileHeader * GetHeader(unsigned char index);
    void ClearHeaders();
    void ClearFileParts();
    void InsertNpc(int index);
    void DeleteNpc(int index);
    unsigned char ReadByte();
    int ReadNumber(unsigned char length);
};

#endif
