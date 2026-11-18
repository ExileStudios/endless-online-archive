#include <vcl.h>
#pragma hdrstop

#include "Npcvalues.h"
#include <Classes.hpp>
#include "Pubfileheader.h"

#pragma package(smart_init)

String Npcvalues_UnusedLiterals1()
{
    "001.enf";
    "";
    "00";
    ".enf";
    "0";
    ".enf";
    "";
    return "";
}

// An empty list; prefix starts the names of its files.
NpcValues::NpcValues(String prefix)
{
    file_prefix = prefix;
    third_unused_text = prefix;
    record_index = -1;
    is_downloading = false;
    is_loaded = false;
    has_bad_record = false;
    header_read = false;
    checksum_high = 0;
    checksum_high = 0;
    record_count = 0;
    second_unused_flag = false;
    third_unused_flag = false;
}

NpcValues::~NpcValues()
{
    second_unused_flag = false;
    third_unused_flag = false;
}

// Reads an NPC .ini file, adding NPCs up to each id met: drops in fours (item, least, most, rate), or chat
// timings and lines. Returns the entries read, or -1 when the file has no lines.
int NpcValues::ImportIni(int format, String path)
{
    int unused_value;
    int second_unused_value;
    int third_unused_value;
    TStringList *lines = new TStringList();
    try
    {
        lines->LoadFromFile(path);
    }
    catch (...)
    {
    }
    if (lines->Count <= 0)
        return -1;
    int entries_read = 0;
    if (format == NpcIniFormat_Drops)
    {
        int line_index = 0;
        for (; lines->Count > line_index; line_index++)
        {
            if (lines->Strings[line_index].Length() < 1)
                continue;
            if (lines->Strings[line_index][1] < '0')
                continue;
            if (lines->Strings[line_index][1] > '9')
                continue;
            int equals_pos = lines->Strings[line_index].Pos("=");
            if (equals_pos <= 0)
                continue;
            String id_part = lines->Strings[line_index].SubString(1, equals_pos - 1);
            String npc_id_text = "";
            for (int i = 1; i <= id_part.Length(); i++)
            {
                if (id_part[i] >= '0')
                {
                    if (id_part[i] <= '9')
                        npc_id_text += id_part[i];
                }
            }
            if (npc_id_text.Length() < 1)
                continue;
            entries_read++;
            int missing = (int)(npc_id_text - (int)records.size());
            // The list grows to the NPC id, each new NPC numbered after the last.
            if (missing > 0)
            {
                for (int added = 0; added < missing; added++)
                {
                    int next_id = records.size() + 1;
                    records.insert(records.end(), NpcValue(next_id));
                }
            }
            int item_id = 0;
            int min_amount = 0;
            int max_amount = 0;
            int rate = 0;
            String values = lines->Strings[line_index];
            values.Delete(1, equals_pos);
            values = KeepNumbers(values);
            int value_start = 1;
            int value_number = 0;
            for (int pos = 1; pos <= values.Length(); pos++)
            {
                if (values[pos] == ',' || pos == values.Length())
                {
                    if (pos == values.Length())
                        pos++;
                    String value = values.SubString(value_start, pos - value_start);
                    value_start = pos;
                    value_number++;
                    if (value.Length() > 0)
                    {
                        if (value_number == 1)
                            item_id = ParseNumber(value);
                        if (value_number == 2)
                            min_amount = ParseNumber(value);
                        if (value_number == 3)
                            max_amount = ParseNumber(value);
                        if (value_number == 4)
                            rate = ParseRate(value);
                    }
                    if (value_number == 4)
                    {
                        records[(unsigned int)(npc_id_text - 1)].AddDrop(item_id, min_amount, max_amount, rate);
                        entries_read++;
                        value_number = 0;
                    }
                }
            }
        }
    }
    if (format == NpcIniFormat_Chats)
    {
        for (int chat_line_index = 0; lines->Count > chat_line_index; chat_line_index++)
        {
            if (lines->Strings[chat_line_index].Length() < 1)
                continue;
            if (lines->Strings[chat_line_index][1] < '0')
                continue;
            if (lines->Strings[chat_line_index][1] > '9')
                continue;
            int dot_pos = lines->Strings[chat_line_index].Pos(".");
            if (dot_pos <= 0)
                continue;
            String chat_id_part = lines->Strings[chat_line_index].SubString(1, dot_pos - 1);
            String chat_npc_id = "";
            int i;
            for (i = 1; i <= chat_id_part.Length(); i++)
            {
                if (chat_id_part[i] >= '0' && chat_id_part[i] <= '9')
                    chat_npc_id += chat_id_part[i];
            }
            if (chat_npc_id.Length() < 1)
                continue;
            int chat_missing = (int)(chat_npc_id - (int)records.size());
            if (chat_missing > 0)
            {
                for (int chat_added = 0; chat_added < chat_missing; chat_added++)
                {
                    int chat_next_id = records.size() + 1;
                    records.insert(records.end(), NpcValue(chat_next_id));
                }
            }
            int chat_pos = lines->Strings[chat_line_index].Pos("chat.");
            if (chat_pos <= 0)
            {
                // A rate key means the npcs.ini layout, which is read again the third way.
                if (lines->Strings[chat_line_index].Pos(".rate") > 0)
                {
                    format = NpcIniFormat_ChatRates;
                    break;
                }
                continue;
            }
            String setting = lines->Strings[chat_line_index];
            setting.Delete(1, chat_pos + 4);
            int setting_equals = setting.Pos("=");
            String setting_name = setting.SubString(1, setting_equals);
            i = 0;
            if (setting_name.Pos("interval") > 0)
                i = 1;
            else if (setting_name.Pos("frequency") > 0)
                i = 2;
            else
            {
                setting_name = ParseNumber(setting_name);
                if (setting_name.Length() > 0)
                    i = 3;
            }
            setting.Delete(1, setting_equals);
            if (i == 1)
                records[(unsigned int)(chat_npc_id - 1)].talk_interval = ParseNumber(setting);
            if (i == 2)
                records[(unsigned int)(chat_npc_id - 1)].talk_rate = ParseNumber(setting);
            if (i == 3)
            {
                entries_read++;
                setting = setting.Trim();
                records[(unsigned int)(chat_npc_id -
                    1)].talk_lines.insert(records[(unsigned int)(chat_npc_id - 1)].talk_lines.end(), setting);
            }
        }
    }
    if (format == NpcIniFormat_ChatRates)
    {
        for (int rate_line_index = 0; lines->Count > rate_line_index; rate_line_index++)
        {
            if (lines->Strings[rate_line_index].Length() < 1)
                continue;
            if (lines->Strings[rate_line_index][1] < '0')
                continue;
            if (lines->Strings[rate_line_index][1] > '9')
                continue;
            int rate_dot_pos = lines->Strings[rate_line_index].Pos(".");
            if (rate_dot_pos <= 0)
                continue;
            String rate_id_part = lines->Strings[rate_line_index].SubString(1, rate_dot_pos - 1);
            String rate_npc_id = "";
            for (int digit_index = 1; digit_index <= rate_id_part.Length(); digit_index++)
            {
                if (rate_id_part[digit_index] >= '0' && rate_id_part[digit_index] <= '9')
                    rate_npc_id += rate_id_part[digit_index];
            }
            if (rate_npc_id.Length() < 1)
                continue;
            int rate_missing = (int)(rate_npc_id - (int)records.size());
            if (rate_missing > 0)
            {
                for (int rate_added = 0; rate_added < rate_missing; rate_added++)
                {
                    int rate_next_id = records.size() + 1;
                    records.insert(records.end(), NpcValue(rate_next_id));
                }
            }
            int first_equals = lines->Strings[rate_line_index].Pos("=");
            if (first_equals <= 0)
                continue;
            String rate_line = lines->Strings[rate_line_index];
            int rate_equals = rate_line.Pos("=");
            String rate_setting_name = rate_line.SubString(1, rate_equals);
            int rate_setting = 0;
            if (rate_setting_name.Pos("chats") > 0)
                continue;
            if (rate_setting_name.Pos("rate") > 0)
                rate_setting = 1;
            else if (rate_setting_name.Pos("frequency") > 0)
                rate_setting = 1;
            else
            {
                rate_setting_name = ParseNumber(rate_setting_name);
                if (rate_setting_name.Length() > 0)
                    rate_setting = 3;
            }
            rate_line.Delete(1, rate_equals);
            if (rate_setting == 1)
            {
                records[(unsigned int)(rate_npc_id - 1)].talk_interval = 0;
                records[(unsigned int)(rate_npc_id - 1)].talk_rate = ParseNumber(rate_line);
            }
            if (rate_setting == 3)
            {
                entries_read++;
                rate_line = rate_line.Trim();
                records[(unsigned int)(rate_npc_id -
                    1)].talk_lines.insert(records[(unsigned int)(rate_npc_id - 1)].talk_lines.end(), rate_line);
            }
        }
    }
    return entries_read;
}

int CopyNpcDrops(int first, int last, int dest)
{
    for (; first != last; dest = dest + 16)
    {
        *(NpcDropItem *)dest = *(NpcDropItem *)first;
        first += 16;
    }
    return dest;
}

int CopyStrings(int first, int last, int dest)
{
    for (int source = first; source != last; dest = dest + 4)
    {
        source += 4;
        *(String *)dest = *(String *)source;
    }
    return dest;
}

// The digits of the text as a number; 0 when it has none.
DEFINE_PARSE_NUMBER(NpcValues)

// A rate with up to two decimals in hundredths, 12.5 as 1250; commas are left out.
int NpcValues::ParseRate(String text)
{
    int rate;
    if (!text.Length())
        return 0;
    if (text.Length() > 1 && text[2] == '.')
        text.Insert("0", 2);
    String number = "";
    for (int i = 1; i <= text.Length(); i++)
    {
        if (text[i] != ',')
            number += text[i];
    }
    if (number.Pos(".") <= 0)
        return 100 * StrToInt(number);
    int dot_pos = number.Pos(".");
    String whole = number.SubString(1, dot_pos - 1);
    number.Delete(1, dot_pos);
    String fraction = number;
    rate = 100 * StrToInt(whole);
    if (fraction.Length() > 0 && fraction[1] != '0')
        rate += 10 * StrToInt(fraction[1]);
    if (fraction.Length() > 1 && fraction[2] != '0')
        rate += StrToInt(fraction[2]);
    return rate;
}

// The text with only its digits, commas and dots.
String NpcValues::KeepNumbers(String text)
{
    String kept = "";
    for (int i = 1; i <= text.Length(); i++)
    {
        if (text[i] == ',' || text[i] == '.')
            kept += text[i];
        else if (text[i] >= '0' && text[i] <= '9')
            kept += text[i];
    }
    return kept;
}

void Npcvalues_UnusedLiterals2(PubfileHeader *object)
{
    "00";
    ".enf";
    "0";
    ".enf";
    "";
    "";
    "serv_drops.epf";
    "";
    "serv_chats.epf";
    "";
    "00";
    ".enf";
    "0";
    ".enf";
    "";
    "";
    "dtn";
    delete object;
}

// Empties the list.
void NpcValues::Clear()
{
    if (Count() >= 1)
    {
        records.clear();
        record_index = -1;
    }
}

NpcValue *NpcValues::GetNpc(int index)
{
    return &records[index];
}

unsigned int NpcValues::Count()
{
    return records.size();
}

void NpcValues::ClearHeaders()
{
    for (int i = 0; i < headers.size(); i++)
        delete headers[i];
    headers.clear();
}
