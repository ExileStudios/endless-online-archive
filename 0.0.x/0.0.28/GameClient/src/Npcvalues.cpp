#include <vcl.h>
#pragma hdrstop

#include "Npcvalues.h"
#include "Classvalues.h"
#include "Packetcontrols.h"
#include <fstream.h>
#include <io.h>
#include <stdio.h>
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

NpcValues::NpcValues()
{
    file_prefix = "./pub/dtn";
    is_downloading = 0;
    is_loaded = 0;
    unread_int_at_64 = -1;
    checksum_high = 0;
    checksum_high = 0;
    record_count = 0;
}

NpcValues::~NpcValues()
{
}

// Reads the checksum and NPC count from the header of the first NPC file.
// With more than 900 NPCs the last part file is read too; the values are kept only when its header matches.
void NpcValues::ReadHeader()
{
    String file_text;
    int first_checksum_high;
    int first_checksum_low;
    int file_handle;
    DWORD file_size;
    char *buffer;
    int total_records;
    String file_name = file_prefix + "001.enf";
    try
    {
        file_handle = FileOpen(file_name.c_str(), 0);
        file_size = FileSeek(file_handle, 0, 2);
        FileSeek(file_handle, 0, 0);
        buffer = new char[file_size + 1];
        FileRead(file_handle, buffer, file_size);
        FileClose(file_handle);
        file_text = buffer;
        file_text.SetLength(file_size);
        delete[] buffer;
        // "ENF".
        if (file_text[1] != 'E' || file_text[2] != 'N' || file_text[3] != 'F')
            return;
        first_checksum_high = DecodeNumber(file_text.SubString(4, 2));
        first_checksum_low = DecodeNumber(file_text.SubString(6, 2));
        total_records = DecodeNumber(file_text.SubString(8, 2));
    }
    catch (...)
    {
        first_checksum_high = 0;
        first_checksum_low = 0;
        total_records = 0;
    }
    if (total_records > 900)
    {
        file_text = "";
        file_name = file_prefix;
        int last_file_number = total_records / PUB_RECORDS_PER_FILE + 1;
        if (last_file_number < 10)
            file_name = file_name + "00" + IntToStr(last_file_number) + ".enf";
        else
            file_name = file_name + "0" + IntToStr(last_file_number) + ".enf";
        try
        {
            file_handle = FileOpen(file_name.c_str(), 0);
            file_size = FileSeek(file_handle, 0, 2);
            FileSeek(file_handle, 0, 0);
            buffer = new char[file_size + 1];
            FileRead(file_handle, buffer, file_size);
            FileClose(file_handle);
            file_text = buffer;
            file_text.SetLength(file_size);
            delete[] buffer;
            if (file_text[1] != 'E' || file_text[2] != 'N' || file_text[3] != 'F')
            {
                checksum_high = 0;
                checksum_low = 0;
                record_count = 0;
            }
            else
            {
                int last_checksum_high = DecodeNumber(file_text.SubString(4, 2));
                int last_checksum_low = DecodeNumber(file_text.SubString(6, 2));
                int last_total_records = DecodeNumber(file_text.SubString(8, 2));
                if (last_checksum_high == first_checksum_high && last_checksum_low == first_checksum_low &&
                    last_total_records == total_records)
                {
                    checksum_high = first_checksum_high;
                    checksum_low = first_checksum_low;
                    record_count = total_records;
                }
            }
        }
        catch (...)
        {
            checksum_high = 0;
            checksum_low = 0;
            record_count = 0;
        }
    }
    else
    {
        checksum_high = first_checksum_high;
        checksum_low = first_checksum_low;
        record_count = total_records;
    }
}

// Writes one part of the NPC file as the server sent it.
void NpcValues::SaveFilePart(int part_number, String data)
{
    String lpFileName = file_prefix;
    if (part_number < 10)
        lpFileName = lpFileName + "00" + IntToStr(part_number) + ".enf";
    else
        lpFileName = lpFileName + "0" + IntToStr(part_number) + ".enf";
    std::ofstream handle;
    handle.open(lpFileName.c_str(), std::ios::binary);
    handle << data.c_str();
    handle.close();
}

// Reads every NPC part file into the list, once until the files change.
void NpcValues::Load()
{
    int file_number;
    int loaded_count;
    int record_total;
    int hFile;
    if (!is_loaded)
    {
        file_number = 1;
        loaded_count = 0;
        record_total = 1;
        do
        {
            String file_text;
            String file_path = file_prefix;
            if (file_number < 10)
                file_path = file_path + "00" + IntToStr(file_number) + ".enf";
            else
                file_path = file_path + "0" + IntToStr(file_number) + ".enf";
            try
            {
                DWORD file_size;
                char *buffer;
                hFile = FileOpen(file_path.c_str(), 0);
                file_size = FileSeek(hFile, 0, 2);
                FileSeek(hFile, 0, 0);
                buffer = new char[file_size + 1];
                FileRead(hFile, buffer, file_size);
                FileClose(hFile);
                file_text = buffer;
                file_text.SetLength(file_size);
                delete[] buffer;
                if (file_text[1] != 'E' || file_text[2] != 'N' || file_text[3] != 'F')
                    return;
                if (file_number == 1)
                {
                    int header_checksum_high = DecodeNumber(file_text.SubString(4, 2));
                    int header_checksum_low = DecodeNumber(file_text.SubString(6, 2));
                    int header_count = DecodeNumber(file_text.SubString(8, 2));
                    int header_byte_at = DecodeNumber(file_text.SubString(10, 1));
                    record_total = header_count;
                }
                file_text.Delete(1, 10);
                for (int i = 0; loaded_count < record_total && i < 900; i++)
                {
                    // Each NPC: a length byte, the name, then 39 bytes of numbers.
                    int name_end = DecodeNumber(file_text.SubString(1, 1)) + 1;
                    AddNpc(Count() + 1, file_text.SubString(2, name_end - 1),
                        DecodeNumber(file_text.SubString(name_end + 1, 2)),
                        DecodeNumber(file_text.SubString(name_end + 3, 1)),
                        DecodeNumber(file_text.SubString(name_end + 4, 2)),
                        DecodeNumber(file_text.SubString(name_end + 6, 2)),
                        DecodeNumber(file_text.SubString(name_end + 8, 2)),
                        DecodeNumber(file_text.SubString(name_end + 10, 2)),
                        DecodeNumber(file_text.SubString(name_end + 12, 3)),
                        DecodeNumber(file_text.SubString(name_end + 15, 2)),
                        DecodeNumber(file_text.SubString(name_end + 17, 2)),
                        DecodeNumber(file_text.SubString(name_end + 19, 2)),
                        DecodeNumber(file_text.SubString(name_end + 21, 2)),
                        DecodeNumber(file_text.SubString(name_end + 23, 2)),
                        DecodeNumber(file_text.SubString(name_end + 25, 2)),
                        DecodeNumber(file_text.SubString(name_end + 27, 1)),
                        DecodeNumber(file_text.SubString(name_end + 28, 2)),
                        DecodeNumber(file_text.SubString(name_end + 30, 2)),
                        DecodeNumber(file_text.SubString(name_end + 32, 2)),
                        DecodeNumber(file_text.SubString(name_end + 34, 2)),
                        DecodeNumber(file_text.SubString(name_end + 36, 1)),
                        DecodeNumber(file_text.SubString(name_end + 37, 3)));
                    loaded_count++;
                    file_text.Delete(1, name_end + 39);
                }
                is_loaded = 1;
            }
            catch (...)
            {
                FileClose(hFile);
                is_loaded = 0;
            }
            file_number++;
        } while (loaded_count < record_total);
    }
}

// Deletes the NPC part files from 001 up to the first one missing, 100 at most.
void NpcValues::DeleteFiles()
{
    char file_found = 1;
    String lpFileName;
    try
    {
        for (int i = 1; i <= 100 && file_found; i++)
        {
            lpFileName = file_prefix;
            if (i < 10)
                lpFileName = lpFileName + "00" + IntToStr(i) + ".enf";
            else
                lpFileName = lpFileName + "0" + IntToStr(i) + ".enf";
            if (!access(lpFileName.c_str(), 0))
                remove(lpFileName.c_str());
            else
                file_found = 0;
        }
    }
    catch (...)
    {
    }
}

void NpcValues::AddNpc(int npc_id, String name, short graphic, short npc_record_byte_at_3, short npc_record_word_at_4,
    short npc_record_word_at_6, short npc_type, short npc_record_word_at_10, int npc_record_triple_at_12,
    short npc_record_word_at_15, short npc_record_word_at_17, short npc_record_word_at_19, short npc_record_word_at_21,
    short npc_record_word_at_23, short npc_record_word_at_25, short npc_record_byte_at_27, short npc_record_word_at_28,
    short npc_record_word_at_30, short npc_record_word_at_32, short npc_record_word_at_34, short npc_record_byte_at_36,
    int npc_record_triple_at_37)
{
    NpcValue handle(npc_id);
    handle.npc_id = npc_id;
    handle.name = name;
    handle.graphic = graphic;
    handle.npc_record_byte_at_3 = npc_record_byte_at_3;
    handle.npc_record_word_at_4 = npc_record_word_at_4;
    handle.npc_record_word_at_6 = npc_record_word_at_6;
    handle.npc_type = npc_type;
    handle.npc_record_word_at_10 = npc_record_word_at_10;
    handle.npc_record_triple_at_12 = npc_record_triple_at_12;
    handle.npc_record_word_at_15 = npc_record_word_at_15;
    handle.npc_record_word_at_17 = npc_record_word_at_17;
    handle.npc_record_word_at_19 = npc_record_word_at_19;
    handle.npc_record_word_at_21 = npc_record_word_at_21;
    handle.npc_record_word_at_23 = npc_record_word_at_23;
    handle.npc_record_word_at_25 = npc_record_word_at_25;
    handle.npc_record_byte_at_27 = npc_record_byte_at_27;
    handle.npc_record_word_at_28 = npc_record_word_at_28;
    handle.npc_record_word_at_30 = npc_record_word_at_30;
    handle.npc_record_word_at_32 = npc_record_word_at_32;
    handle.npc_record_word_at_34 = npc_record_word_at_34;
    handle.npc_record_byte_at_36 = npc_record_byte_at_36;
    handle.npc_record_triple_at_37 = npc_record_triple_at_37;
    records.insert(records.end(), handle);
}

DEFINE_CLEAR(NpcValues)

int NpcValues::GetGraphic(int npc_id)
{
    int graphic = -1;
    vector<NpcValue>::iterator it;
    for (it = records.begin(); it != records.end(); it++)
    {
        if (npc_id == it->npc_id)
        {
            graphic = it->graphic;
            break;
        }
    }
    return graphic;
}

String NpcValues::GetName(int npc_id)
{
    String name;
    for (vector<NpcValue>::iterator it = records.begin(); it != records.end(); it++)
    {
        if (npc_id == it->npc_id)
        {
            name = it->name;
            break;
        }
    }
    return name;
}

// The two type numbers of an NPC, both 0 when the id is not in the list.
NpcTypeInfo NpcValues::GetType(int npc_id)
{
    NpcTypeInfo type_pair;
    type_pair.npc_type = NpcType_Friendly;
    type_pair.npc_record_word_at_10 = 0;
    vector<NpcValue>::iterator it;
    for (it = records.begin(); it != records.end(); it++)
    {
        if (npc_id == it->npc_id)
        {
            type_pair.npc_type = it->npc_type;
            type_pair.npc_record_word_at_10 = it->npc_record_word_at_10;
            break;
        }
    }
    return type_pair;
}

unsigned int NpcValues::Count()
{
    return records.size();
}

DEFINE_DECODE_NUMBER(NpcValues)
