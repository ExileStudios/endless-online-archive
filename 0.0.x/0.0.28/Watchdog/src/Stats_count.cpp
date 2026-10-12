#include <vcl.h>
#pragma hdrstop

#include "Stats_count.h"
#include "Serial.h"
#include "Settings.h"
#include "Settingshtml.h"

#pragma package(smart_init)

#include <io.h>

// Loads templates/count.eht when it is there.
CountStats::CountStats(SerialKey *serial, Settings *server_settings, SettingsHTM *htm_settings)
{
    settings = server_settings;
    html_settings = htm_settings;
    serial_key = serial;
    players = "0";
    template_lines = new TStringList();
    page_lines = new TStringList();
    has_template = 0;
    if (!access("./templates/count.eht", 0))
    {
        has_template = 1;
        template_lines->LoadFromFile("./templates/count.eht");
    }
}

CountStats::~CountStats()
{
}

// Writes count.html to the outputpath: the template with its tags filled in.
void CountStats::GenerateHTML()
{
    page_lines->Clear();
    for (int i = 0; i < template_lines->Count; i++)
        page_lines->Add(template_lines->Strings[i]);
    for (int j = 0; j < page_lines->Count; j++)
        FillTemplateLine(j);
    String path = settings->GetOutputPath();
    if (path.Length() > 0)
    {
        if (path[path.Length()] != '\\')
        {
            if (path[path.Length()] != '/')
                path = path + (char)92;
        }
    }
    path = path + "count.html";
    try
    {
        page_lines->SaveToFile(path);
    }
    catch (...)
    {
    }
}

// Replaces the {PLAYERS} tag of one line of the page.
void CountStats::FillTemplateLine(int line_index)
{
    char in_tag;
    char changed;
    int tag_start;
    int tag_length;
    int i;
    in_tag = 0;
    changed = 0;
    tag_start = 0;
    tag_length = 0;
    String line = page_lines->Strings[line_index];
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
                if (tag == "{PLAYERS}")
                    line.Insert(players, tag_start);
                // after each tag the line is read again from the start
                i = 1;
                in_tag = 0;
                changed = 1;
                tag_start = 0;
                tag_length = 0;
            }
        }
    }
    if (changed)
        page_lines->Strings[line_index] = line;
}
