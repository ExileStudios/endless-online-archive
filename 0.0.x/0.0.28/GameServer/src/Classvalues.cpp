#include <vcl.h>
#pragma hdrstop

#include "Classvalues.h"
#include "Classvalue.h"
#include "Constants.h"

#pragma package(smart_init)

ClassValues::ClassValues()
{
    encode_scratch = new char[8];
    record_index = -1;
    loaded = 0;
    file_id = 0;
    string_list = new TStringList();
    LoadClasses();
}

ClassValues::~ClassValues()
{
}

// Reads pub/dat001.ecf and the files after it until every class is loaded.
void ClassValues::LoadClasses()
{
    int file;
    int count;
    int total;
    int file_handle;
    if (!loaded)
    {
        file = 1;
        count = 0;
        total = 1;
        do
        {
            String data;
            String path = "./pub/dat";
            if (file < 10)
                path = path + "00" + IntToStr(file) + ".ecf";
            else
                path = path + "0" + IntToStr(file) + ".ecf";
            try
            {
                int size;
                char *file_buffer;
                file_handle = FileOpen(path.c_str(), 0);
                size = FileSeek(file_handle, 0, 2);
                FileSeek(file_handle, 0, 0);
                file_buffer = new char[size + 1];
                FileRead(file_handle, file_buffer, size);
                FileClose(file_handle);
                data = file_buffer;
                data.SetLength(size);
                delete[] file_buffer;
                // a file without the ECF header ends the loading
                if (data[1] != 'E' || data[2] != 'C' || data[3] != 'F')
                    return;
                string_list->Add(data);
                // .. the first file's header holds the number of classes
                if (file == 1)
                {
                    rid[0] = DecodeNumber(data.SubString(4, 2));
                    rid[1] = DecodeNumber(data.SubString(6, 2));
                    int parsed = DecodeNumber(data.SubString(8, 2));
                    int version = DecodeNumber(data.SubString(10, 1));
                    total = parsed;
                    num_records = parsed;
                }
                data.Delete(1, 10);
                for (int j = 0; count < total && j < 250; j++)
                {
                    int namelen = DecodeNumber(data.SubString(1, 1));
                    AddClass(GetCount() + 1, DecodeNumber(data.SubString(namelen + 2, 1)), data.SubString(2, namelen),
                        DecodeNumber(data.SubString(namelen + 3, 1)), DecodeNumber(data.SubString(namelen + 4, 2)),
                        DecodeNumber(data.SubString(namelen + 6, 2)), DecodeNumber(data.SubString(namelen + 8, 2)),
                        DecodeNumber(data.SubString(namelen + 10, 2)), DecodeNumber(data.SubString(namelen + 12, 2)),
                        DecodeNumber(data.SubString(namelen + 14, 2)));
                    count++;
                    data.Delete(1, namelen + 15);
                }
            }
            catch (...)
            {
                FileClose(file_handle);
                loaded = 0;
            }
            file++;
        } while (count < total);
        file_id = file - 1;
        loaded = 1;
    }
    return;
}

void ClassValues::AddClass(int id, int parent_type, String name, short stat_group, short str, short intl, short wis,
    short agi, short con, short cha)
{
    ClassValue record(id);
    record.name = name;
    record.parent_type = parent_type;
    record.stat_group = stat_group;
    record.str = str;
    record.intl = intl;
    record.wis = wis;
    record.agi = agi;
    record.con = con;
    record.cha = cha;
    record_list.insert(record_list.end(), record);
}

ClassValue &Classvalues_Slot(vector<ClassValue> &list, int index)
{
    return list[index];
}

// True when the class is the required one, or comes from it through its parents; 0 requires nothing.
char ClassValues::ClassMatches(int class_id, int class_requirement)
{
    if (!class_requirement)
        return 1;
    while (class_id > 0)
    {
        if (class_id == class_requirement)
            return 1;
        class_id = record_list[class_id - 1].parent_type;
    }
    return 0;
}

// The class at index; one out of range gives the first class.
ClassValue ClassValues::GetByIndex(int index)
{
    if (index < 0 || record_list.size() - 1 < (unsigned int)index)
        index = 0;
    return record_list[index];
}

int ClassValues::GetCount()
{
    return record_list.size();
}

// Reads a number of up to four bytes; a 254 or a 0 ends it.
DEFINE_DECODE_NUMBER(ClassValues)
