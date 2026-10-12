#include <vcl.h>
#pragma hdrstop

#include "Stats_guilds.h"
#include "Serial.h"
#include "Settings.h"
#include "Settingshtml.h"

#pragma package(smart_init)

#include <io.h>

// Loads templates/guilds.eht and finds its {BEGIN_LIST} and {END_LIST} lines.
GuildStats::GuildStats(SerialKey *serial, Settings *server_settings, SettingsHTM *htm_settings)
{
    char begin_found;
    settings = server_settings;
    html_settings = htm_settings;
    serial_key = serial;
    tags = new TStringList();
    names = new TStringList();
    exps = new TStringList();
    exp_highs = new TStringList();
    members = new TStringList();
    guild_count = 0;
    template_lines = new TStringList();
    page_lines = new TStringList();
    has_template = 0;
    if (!access("./templates/guilds.eht", 0))
    {
        has_template = 1;
        template_lines->LoadFromFile("./templates/guilds.eht");
    }
    if (has_template)
    {
        list_found = 0;
        begin_found = 0;
        for (int i = 0; template_lines->Count > i && !list_found; i++)
        {
            if (!begin_found)
            {
                if (IsListMarker(template_lines->Strings[i], 0))
                {
                    list_begin = i;
                    begin_found = 1;
                }
            }
            else
            {
                if (IsListMarker(template_lines->Strings[i], 1))
                {
                    list_end = i;
                    list_found = 1;
                }
            }
        }
    }
}

GuildStats::~GuildStats()
{
}

// Writes guilds.html to the outputpath.
void GuildStats::GenerateHTML()
{
    page_lines->Clear();
    BuildPage();
    String path = settings->GetOutputPath();
    if (path.Length() > 0)
    {
        if (path[path.Length()] != '\\')
        {
            if (path[path.Length()] != '/')
                path = path + (char)92;
        }
    }
    path = path + "guilds.html";
    try
    {
        page_lines->SaveToFile(path);
    }
    catch (...)
    {
    }
}

void GuildStats::ClearList()
{
    tags->Clear();
    names->Clear();
    exps->Clear();
    exp_highs->Clear();
    members->Clear();
}

// Adds a guild, keeping the list sorted by experience, highest first.
void GuildStats::AddGuild(String tag, String guild_name, int experience, int exp_high, int member_count)
{
    int i;
    for (i = 0; i < exps->Count; i++)
    {
        if (StrToInt(exps->Strings[i]) < experience)
            break;
    }
    tags->Insert(i, tag);
    names->Insert(i, guild_name);
    exps->Insert(i, IntToStr(experience));
    exp_highs->Insert(i, IntToStr(exp_high));
    members->Insert(i, IntToStr(member_count));
}

// Fills a tag of one list line for the guild at list_index.
String GuildStats::FillListLine(String template_line)
{
    char in_tag;
    char changed;
    int tag_start;
    int tag_length;
    int i;
    String line = template_line;
    in_tag = 0;
    changed = 0;
    tag_start = 0;
    tag_length = 0;
    for (i = 1; i <= line.Length(); i++)
    {
        if (!in_tag)
        {
            if (line[i] == '{')
            {
                in_tag = 1;
                tag_length = 1;
                tag_start = i;
            }
        }
        else
        {
            tag_length++;
            if (line[i] == '}')
            {
                String tag = line.SubString(tag_start, tag_length);
                line.Delete(tag_start, tag_length);
                if (names->Count > 0)
                {
                    if (tag == "{TAG}")
                        line.Insert(tags->Strings[list_index], tag_start);
                    if (tag == "{NAME}")
                        line.Insert(names->Strings[list_index], tag_start);
                    if (tag == "{GUILD}")
                        line.Insert(names->Strings[list_index], tag_start);
                    if (tag == "{MEMBERS}")
                        line.Insert(members->Strings[list_index], tag_start);
                    if (tag == "{EXP}")
                        line.Insert(exps->Strings[list_index], tag_start);
                    if (tag == "{EXPHI}")
                        line.Insert(exp_highs->Strings[list_index], tag_start);
                }
                i = line.Length();
                in_tag = 1;
            }
        }
    }
    return line;
}

// Builds the page: the list lines once for every guild, and {COUNT} and {GENDATE} on the lines around them.
void GuildStats::BuildPage()
{
    for (int i = 0; i < template_lines->Count; i++)
    {
        if (i > 1400)  // a template past 1400 lines closes the Watchdog
            Application->Terminate();
        // .. the list block, once for every guild
        if (i == list_begin && has_template)
        {
            list_index = 0;
            do
            {
                for (int j = list_begin; j < list_end && template_lines->Count > j; j++)
                {
                    page_lines->Add(FillListLine(template_lines->Strings[j]));
                    if (list_begin + 100 < j)  // and so does a list past 100 lines
                        Application->Terminate();
                }
                list_index++;
            } while (list_index < guild_count);
        }
        char in_tag = 0;
        char changed = 0;
        int tag_start = 0;
        int tag_length = 0;
        String line = template_lines->Strings[i];
        for (int k = 1; k <= line.Length(); k++)
        {
            if (!in_tag)
            {
                if (line[k] == '{')
                {
                    in_tag = 1;
                    tag_length = 1;
                    tag_start = k;
                }
            }
            else
            {
                tag_length++;
                if (line[k] == '}')
                {
                    String tag = line.SubString(tag_start, tag_length);
                    line.Delete(tag_start, tag_length);
                    if (tag == "{COUNT}")
                        line.Insert(IntToStr(guild_count), tag_start);
                    if (tag == "{GENDATE}")
                        line.Insert(DateToStr(Date()) + " " + TimeToStr(Time()), tag_start);
                    changed = 1;
                }
            }
        }
        if (!changed)
            page_lines->Add(template_lines->Strings[i]);
        else
            page_lines->Add(line);
    }
}

DEFINE_IS_LIST_MARKER(GuildStats)
