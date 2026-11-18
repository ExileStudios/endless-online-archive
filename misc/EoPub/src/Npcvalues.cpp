#include <vcl.h>
#pragma hdrstop

#include "Npcvalues.h"
#include "Npcvalue.h"
#include "Pubfileheader.h"

#pragma package(smart_init)

NpcValues::NpcValues(String prefix, char keep_parts)
{
    file_prefix = prefix + "dtn";
    pub_folder = prefix;
    file_parts = new TStringList();
    keep_file_parts = keep_parts;
    is_downloading = false;
    is_loaded = false;
    has_bad_record = false;
    header_read = false;
    has_bad_drop = false;
    has_bad_message = false;
    checksum_high = 0;
    checksum_high = 0;
    record_count = 0;
    drops_loaded = false;
    chats_loaded = false;
}

NpcValues::~NpcValues()
{
    delete file_parts;
}

void Npcvalues_UnusedLiterals1()
{
    "001.enf";
    "";
    "00";
    ".enf";
    "0";
    ".enf";
    "";
    "";
}

// Loads the NPCs from the numbered .enf files, up to 900 from each, until the count the headers give is read.
void NpcValues::Load()
{
    int file_handle;
    int file_size;
    short file_number;
    char *buffer;
    if (!is_loaded)
    {
        Clear();
        ClearHeaders();
        ClearFileParts();
        file_number = 0;
        records_read = 0;
        records_expected = 0;
        do
        {
            file_number++;
            if (file_number < 10)
                file_name = "00" + IntToStr(file_number) + ".enf";
            else
                file_name = "0" + IntToStr(file_number) + ".enf";
            file_path = file_prefix + file_name;
            try
            {
                file_handle = FileOpen(file_path.c_str(), fmOpenRead);
                file_size = FileSeek(file_handle, 0, 2);
                if (file_size < 0)
                {
                    header_read = true;
                    return;
                }
                FileSeek(file_handle, 0, 0);
                buffer = new char[file_size + 1];
                FileRead(file_handle, buffer, file_size);
                FileClose(file_handle);
                data = buffer;
                data.SetLength(file_size);
                delete[] buffer;
                if (data.Length() < 14)
                    return;
                if (data[1] != 'E' || data[2] != 'N' || data[3] != 'F')
                    return;
                if (keep_file_parts)
                    file_parts->Add(data);
                read_position = 4;
                PubfileHeader *header = new PubfileHeader(file_name);
                header->file_length = data.Length();
                header->checksum_high = ReadNumber(2);
                header->checksum_low = ReadNumber(2);
                header->record_count = ReadNumber(2);
                header->version = ReadByte();
                headers.insert(headers.end(), header);
                // The first part's header gives the count of records in all the parts.
                if (!records_read)
                    records_expected = header->record_count;
                int i;
                for (i = 0; records_read < records_expected && i < 900; i++)
                {
                    unsigned char name_length = ReadByte();
                    if (name_length > 50)
                    {
                        has_bad_record = true;
                        header->load_state = 3;
                    }
                    String npc_name = data.SubString(read_position, name_length);
                    NpcValue *npc = new NpcValue(Count() + 1);
                    npc->name = npc_name;
                    read_position += name_length;
                    npc->graphic = ReadNumber(2);
                    npc->race = ReadByte();
                    npc->boss = ReadNumber(2);
                    npc->child = ReadNumber(2);
                    npc->npc_type = ReadNumber(2);
                    npc->behavior_id = ReadNumber(2);
                    npc->hp = ReadNumber(3);
                    npc->tp = ReadNumber(2);
                    npc->min_damage = ReadNumber(2);
                    npc->max_damage = ReadNumber(2);
                    npc->accuracy = ReadNumber(2);
                    npc->evade = ReadNumber(2);
                    npc->armor = ReadNumber(2);
                    npc->return_damage = ReadByte();
                    npc->element = ReadNumber(2);
                    npc->element_damage = ReadNumber(2);
                    npc->element_weakness = ReadNumber(2);
                    npc->element_weakness_damage = ReadNumber(2);
                    npc->level = ReadByte();
                    npc->experience = ReadNumber(3);
                    records.insert(records.end(), npc);
                    header->records_read++;
                    records_read++;
                    if (read_position - 1 == data.Length())
                    {
                        if (header->load_state != 3)
                            header->load_state = 0;
                        break;
                    }
                }
                data = "";
            }
            catch (...)
            {
                is_loaded = false;
                return;
            }
        } while (records_read < records_expected);
        is_loaded = true;
    }
}

// Reads serv_drops.epf into each NPC's drops; a drop for an NPC past the list marks the file bad.
void NpcValues::LoadDrops()
{
    int handle;
    int i;
    int file_length;
    char *buffer;
    if (!drops_loaded)
    {
        drops_read = 0;
        int unused_value = 0;
        drops_path = pub_folder + "serv_drops.epf";
        try
        {
            handle = FileOpen(drops_path.c_str(), fmOpenRead);
            file_length = FileSeek(handle, 0, 2);
            if (file_length < 0)
                return;
            FileSeek(handle, 0, 0);
            buffer = new char[file_length + 1];
            FileRead(handle, buffer, file_length);
            FileClose(handle);
            data = buffer;
            data.SetLength(file_length);
            delete[] buffer;
            if (data.Length() < 14)
                return;
            if (data[1] != 'E' || data[2] != 'D' || data[3] != 'F')
                return;
            // The drop records start after the file's mark and header.
            read_position = 11;
            do
            {
                int npc_id = ReadNumber(2);
                int drop_count = ReadNumber(2);
                for (i = 0; i < drop_count; i++)
                {
                    int item_id = ReadNumber(2);
                    int min_amount = ReadNumber(3);
                    int max_amount = ReadNumber(3);
                    int rate = ReadNumber(2);
                    // A drop for an NPC past the list is left out and marks the file.
                    if (records.size() >= npc_id)
                    {
                        drops_read++;
                        records[npc_id - 1]->AddDrop(item_id, min_amount, max_amount, rate);
                    }
                    else
                        has_bad_drop = true;
                }
            } while ((unsigned int)(read_position + 12) < data.Length());
            drops_loaded = true;
        }
        catch (...)
        {
            drops_loaded = false;
            return;
        }
    }
}

// Reads serv_chats.epf into each NPC's chat lines and talk rate; a line for an NPC past the list marks it bad.
void NpcValues::LoadChats()
{
    int handle;
    int file_length;
    char *buffer;
    if (!chats_loaded)
    {
        messages_read = 0;
        int unused_value = 0;
        chats_path = pub_folder + "serv_chats.epf";
        try
        {
            handle = FileOpen(chats_path.c_str(), fmOpenRead);
            file_length = FileSeek(handle, 0, 2);
            if (file_length < 0)
                return;
            FileSeek(handle, 0, 0);
            buffer = new char[file_length + 1];
            FileRead(handle, buffer, file_length);
            FileClose(handle);
            data = buffer;
            data.SetLength(file_length);
            delete[] buffer;
            if (data.Length() < 14)
                return;
            if (data[1] != 'E' || data[2] != 'T' || data[3] != 'F')
                return;
            // The chat records start after the file's mark and header.
            read_position = 11;
            do
            {
                int npc_id = ReadNumber(2);
                int interval = ReadByte();
                int rate = ReadByte();
                int message_count = ReadByte();
                for (int i = 0; i < message_count; i++)
                {
                    unsigned char message_length = ReadByte();
                    String message = data.SubString(read_position, message_length);
                    read_position += message_length;
                    if (records.size() >= npc_id)
                    {
                        messages_read++;
                        records[npc_id - 1]->AddMessage(interval, rate, message);
                    }
                    else
                        has_bad_message = true;
                }
                if (records[npc_id - 1]->drop_rate_total < 10000)
                    records[npc_id - 1]->drop_rate_total = 10000;
            } while ((unsigned int)(read_position + 6) < data.Length());
            chats_loaded = true;
        }
        catch (...)
        {
            chats_loaded = false;
            return;
        }
    }
}

void Npcvalues_UnusedLiterals2()
{
    "00";
    ".enf";
    "0";
    ".enf";
    "";
    "";
}

// Reads the NPC file, its drops and its chat lines again from the folder given.
void NpcValues::LoadFrom(String folder)
{
    file_prefix = folder + "dtn";
    Clear();
    is_loaded = false;
    has_bad_record = false;
    header_read = false;
    Load();
    drops_loaded = false;
    chats_loaded = false;
    LoadDrops();
    LoadChats();
}

void Npcvalues_UnusedLiterals3()
{
    "";
}

// Deletes every NPC record with its drops and messages.
void NpcValues::Clear()
{
    for (vector<NpcValue *>::iterator it = records.begin(); it != records.end(); it++)
    {
        (*it)->drops.clear();
        (*it)->talk_lines.clear();
        delete *it;
    }
    records.clear();
}

NpcValue *NpcValues::GetNpc(int index)
{
    if (index < 0)
        index = 0;
    if (records.size() > index)
        return records[index];
    return 0;
}

int NpcValues::Count()
{
    return records.size();
}

char NpcValues::HeaderCount()
{
    return headers.size();
}

PubfileHeader *NpcValues::GetHeader(unsigned char index)
{
    return headers[index];
}

DEFINE_CLEAR_HEADERS(NpcValues)

void NpcValues::ClearFileParts()
{
    file_parts->Clear();
}

void NpcValues::InsertNpc(int index)
{
    NpcValue *npc = new NpcValue(index + 1);
    records.insert(records.begin() + index, npc);
}

void NpcValues::DeleteNpc(int index)
{
    delete records[index];
    records.erase(records.begin() + index);
}

DEFINE_NPC_READ_BYTE(NpcValues)

// Reads a number of length bytes at the read position, each a base-253 digit plus one; a 254 ends it early.
DEFINE_READ_NUMBER(NpcValues)
