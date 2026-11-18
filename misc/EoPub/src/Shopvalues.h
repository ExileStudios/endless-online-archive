#ifndef ShopvaluesH
#define ShopvaluesH

#include <new>
#include <vector.h>

struct PubfileHeader;
struct ShopValue;

// The shops read from serv_shops.epf in the pub folder.
struct ShopValues
{
    bool is_loaded;
    bool has_bad_record;
    bool header_read;
    String file_name;
    String file_path;
    int records_read;
    int record_count;
    String data;
    int read_position;
    vector<ShopValue *> records;
    vector<PubfileHeader *> headers;
    String file_prefix;

    ShopValues(String prefix);
    ~ShopValues();
    void Load();
    void LoadFrom(String folder);
    void Clear();
    ShopValue * GetShop(int index);
    int Count();
    char HeaderCount();
    PubfileHeader * GetHeader(unsigned char index);
    void ClearHeaders();
    void InsertShop(int index);
    void DeleteShop(int index);
    char ReadByte();
    int ReadNumber(unsigned char length);
};

#endif
