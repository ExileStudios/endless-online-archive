#ifndef ShopvaluesH
#define ShopvaluesH

#include <new>
#include <vector.h>

struct PubfileHeader;

#include "Shopvalue.h"

// The shops read from shops.ini.
struct ShopValues
{
    String value_text;
    bool is_loaded;
    bool has_bad_record;
    bool header_read;
    String file_name;
    String file_path;
    int records_read;
    int record_count;
    String data;
    int read_position;
    vector<ShopValue> records;
    int record_index;
    vector<PubfileHeader *> headers;
    String file_prefix;

    ShopValues(String prefix);
    ~ShopValues();
    int ImportIni(int format, String path);
    int ReadNextNumber();
    int ParseNumber(String text);
    ShopValue * GetShop(int index);
    int Count();
};

int CopyShopItems(int first, int last, int dest);

#endif
