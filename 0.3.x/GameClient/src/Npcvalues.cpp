#include <vcl.h>
#pragma hdrstop

#include "Npcvalues.h"
#include "Classvalues.h"
#include "Packetcontrols.h"
#include <io.h>
#include <stdio.h>
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

// Reads the checksum and record count from the head of the first npc file; a missing file is marked.
void NpcValues::ReadHeader()
{
    int file_handle;
    char *header_bytes;
    int file_length;
    String header_path = file_prefix + "001.enf";
    try
    {
        file_handle = FileOpen(header_path.c_str(), fmOpenRead);
        file_length = FileSeek(file_handle, 0, 2);
        if (file_length < 0)
        {
            header_read = true;
            return;
        }
        FileSeek(file_handle, 0, 0);
        header_bytes = new char[11];
        FileRead(file_handle, header_bytes, 10);
        FileClose(file_handle);
        data = header_bytes;
        data.SetLength(file_length);
        delete[] header_bytes;
        if (data.Length() < 10)
            return;
        if (data[1] != 'E' || data[2] != 'N' || data[3] != 'F')
            return;
        read_position = 4;
        checksum_high = DecodeNumber(2);
        checksum_low = DecodeNumber(2);
        record_count = DecodeNumber(2);
    }
    catch (...)
    {
        checksum_high = 0;
        checksum_low = 0;
        record_count = 0;
    }
}

// Writes one part of the NPC file as the server sent it.
void NpcValues::SaveFilePart(int part_number, String data)
{
    String lp_file_name = file_prefix;
    if (part_number < 10)
        lp_file_name = lp_file_name + "00" + IntToStr(part_number) + ".enf";
    else
        lp_file_name = lp_file_name + "0" + IntToStr(part_number) + ".enf";
    basic_ofstream<char, char_traits<char> > handle;
    handle.open(lp_file_name.c_str(), std::ios::binary);
    handle << data.c_str();
    handle.close();
}

// Loads the npc records from the numbered .enf files, checking each file's mark and reading up to 900 records from
// each, until the count the headers give is read.
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
        // The file comes in parts numbered from 001, read until the record count of the header.
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
                header->checksum_high = DecodeNumber(2);
                header->checksum_low = DecodeNumber(2);
                header->record_count = DecodeNumber(2);
                header->version = ReadByte();
                headers.insert(headers.end(), header);
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
                    npc->graphic = DecodeNumber(2);
                    npc->race = ReadByte();
                    npc->boss = DecodeNumber(2);
                    npc->child = DecodeNumber(2);
                    npc->npc_type = DecodeNumber(2);
                    npc->behavior_id = DecodeNumber(2);
                    npc->hp = DecodeNumber(3);
                    npc->tp = DecodeNumber(2);
                    npc->min_damage = DecodeNumber(2);
                    npc->max_damage = DecodeNumber(2);
                    npc->accuracy = DecodeNumber(2);
                    npc->evade = DecodeNumber(2);
                    npc->armor = DecodeNumber(2);
                    npc->return_damage = ReadByte();
                    npc->element = DecodeNumber(2);
                    npc->element_damage = DecodeNumber(2);
                    npc->element_weakness = DecodeNumber(2);
                    npc->element_weakness_damage = DecodeNumber(2);
                    npc->level = ReadByte();
                    npc->experience = DecodeNumber(3);
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

// Deletes the NPC part files from 001 up to the first one missing, 100 at most.
void NpcValues::DeleteFiles()
{
    char file_found = true;
    String lp_file_name;
    try
    {
        for (int i = 1; i <= 100 && file_found; i++)
        {
            lp_file_name = file_prefix;
            if (i < 10)
                lp_file_name = lp_file_name + "00" + IntToStr(i) + ".enf";
            else
                lp_file_name = lp_file_name + "0" + IntToStr(i) + ".enf";
            if (!access(lp_file_name.c_str(), 0))
                remove(lp_file_name.c_str());
            else
                file_found = false;
        }
    }
    catch (...)
    {
    }
}

// Deletes every npc record with its drops and messages.
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

// Finds an npc by its number or its name: first the exact name, then a name holding the text, then the closest
// name by score; none for under three letters.
NpcValue *NpcValues::FindByName(String text)
{
    vector<NpcValue *>::iterator it;
    int number = ParseNumber(text);
    if (number >= 0)
        return GetNpc(number);
    if (text.Length() < 3)
        return 0;
    text = LowerCase(text);
    for (it = records.begin(); it != records.end(); it++)
    {
        if (LowerCase((*it)->name) == text)
            return *it;
    }
    for (it = records.begin(); it != records.end(); it++)
    {
        if (LowerCase((*it)->name).Pos(text) > 0)
            return *it;
    }
    for (it = records.begin(); it != records.end(); it++)
    {
        if (StripSpaces((*it)->name).Pos(text) > 0)
            return *it;
    }
    if (records.size() > 0)
    {
        int best_score = 0;
        NpcValue *best = NULL;
        for (it = records.begin(); it != records.end(); it++)
        {
            int score = GetMatchScore((*it)->name, text);
            if (score >= 50 && score > best_score)
            {
                best_score = score;
                best = *it;
            }
        }
        if (best)
            return best;
    }
    return 0;
}

DEFINE_GET_MATCH_SCORE(NpcValues)

// Reads the npc file again from the folder given: the prefix of its file names, then a fresh load.
void NpcValues::LoadFrom(String folder)
{
    file_prefix = folder + "dtn";
    Clear();
    is_loaded = false;
    has_bad_record = false;
    header_read = false;
    Load();
}

DEFINE_STRIP_SPACES(NpcValues)

DEFINE_PARSE_NUMBER(NpcValues)

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

DEFINE_CLEAR_HEADERS(NpcValues)

void NpcValues::ClearFileParts()
{
    file_parts->Clear();
}

// Reads one byte at the read position, stored plus one; a value past 252 reads as 0.
unsigned char NpcValues::ReadByte()
{
    unsigned char value;
    try
    {
        value = data[read_position];
        value--;
    }
    catch (...)
    {
        value = 0;
    }
    read_position++;
    if (value > 252)
        value = 0;
    return value;
}

DEFINE_DECODE_NUMBER(NpcValues)
