#include <vcl.h>
#pragma hdrstop

#include "Stats_top100.h"
#include "Serial.h"
#include "Settings.h"
#include "Settingshtml.h"

#pragma package(smart_init)

#include <io.h>

// Loads templates/top100.eht and finds its {BEGIN_LIST} and {END_LIST} lines.
Top100Stats::Top100Stats(SerialKey *serial, Settings *server_settings, SettingsHTM *htm_settings)
{
    char begin_found;
    settings = server_settings;
    html_settings = htm_settings;
    serial_key = serial;
    names = new TStringList();
    titles = new TStringList();
    exps = new TStringList();
    levels = new TStringList();
    genders = new TStringList();
    admin_levels = new TStringList();
    player_count = 0;
    template_lines = new TStringList();
    page_lines = new TStringList();
    has_template = 0;
    if (!access("./templates/top100.eht", 0))
    {
        has_template = 1;
        template_lines->LoadFromFile("./templates/top100.eht");
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

Top100Stats::~Top100Stats()
{
}

// Writes top100.html to the outputpath.
void Top100Stats::GenerateHTML()
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
    path = path + "top100.html";
    try
    {
        page_lines->SaveToFile(path);
    }
    catch (...)
    {
    }
}

void Top100Stats::ClearList()
{
    names->Clear();
    titles->Clear();
    levels->Clear();
    exps->Clear();
    genders->Clear();
    admin_levels->Clear();
}

// Adds a player, keeping the list sorted by experience, highest first.
void Top100Stats::AddPlayer(String name, String title, int experience, int level, int gender, int admin_level)
{
    int i;
    for (i = 0; i < exps->Count; i++)
    {
        if (StrToInt(exps->Strings[i]) < experience)
            break;
    }
    names->Insert(i, name);
    titles->Insert(i, title);
    exps->Insert(i, IntToStr(experience));
    levels->Insert(i, IntToStr(level));
    genders->Insert(i, IntToStr(gender));
    admin_levels->Insert(i, IntToStr(admin_level));
}

// Builds the page: the list lines once for every player, and {COUNT} and {GENDATE} on the lines around them.
void Top100Stats::BuildPage()
{
    for (int i = 0; i < template_lines->Count; i++)
    {
        if (i > 1400)  // a template past 1400 lines closes the Watchdog
            Application->Terminate();
        // .. the list block, once for every player
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
            } while (list_index < player_count);
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
                        line.Insert(IntToStr(player_count), tag_start);
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

DEFINE_IS_LIST_MARKER(Top100Stats)

// Fills a tag of one list line for the player at list_index.
String Top100Stats::FillListLine(String template_line)
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
                    if (tag == "{NAME}")
                        line.Insert(names->Strings[list_index], tag_start);
                    if (tag == "{TITLE}")
                        line.Insert(StripTags(titles->Strings[list_index]), tag_start);
                    if (tag == "{LEVEL}")
                        line.Insert(levels->Strings[list_index], tag_start);
                    if (tag == "{EXP}")
                        line.Insert(exps->Strings[list_index], tag_start);
                    if (tag == "{GENDER}")
                    {
                        if (genders->Strings[list_index] == "0")
                            line.Insert("Female", tag_start);
                        else
                            line.Insert("Male", tag_start);
                    }
                }
                i = line.Length();
                in_tag = 1;
            }
        }
    }
    return line;
}

DEFINE_STRIP_TAGS(Top100Stats)
