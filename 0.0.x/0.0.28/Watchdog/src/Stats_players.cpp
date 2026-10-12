#include <vcl.h>
#pragma hdrstop

#include "Stats_players.h"
#include "Serial.h"
#include "Settings.h"
#include "Settingshtml.h"

#pragma package(smart_init)

#include <io.h>

// Loads templates/playerlist.eht and finds its {BEGIN_LIST} and {END_LIST} lines.
PlayerStats::PlayerStats(SerialKey *serial, Settings *server_settings, SettingsHTM *htm_settings)
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
    ranked_names = new TStringList();
    ranked_titles = new TStringList();
    ranked_exps = new TStringList();
    ranked_levels = new TStringList();
    ranked_genders = new TStringList();
    ranked_admin_levels = new TStringList();
    player_count = 0;
    template_lines = new TStringList();
    page_lines = new TStringList();
    has_template = 0;
    if (!access("./templates/playerlist.eht", 0))
    {
        has_template = 1;
        template_lines->LoadFromFile("./templates/playerlist.eht");
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

// Writes playerlist.html, the players sorted by name.
void PlayerStats::GenerateHTML()
{
    page_lines->Clear();
    BuildPage((char)1);
    String path = settings->GetOutputPath();
    if (path.Length() > 0)
    {
        if (path[path.Length()] != '\\')
        {
            if (path[path.Length()] != '/')
                path = path + (char)92;
        }
    }
    path = path + "playerlist.html";
    try
    {
        page_lines->SaveToFile(path);
    }
    catch (...)
    {
    }
}

// Writes playerlist_exp.html, the players sorted by experience.
void PlayerStats::GenerateExpHTML()
{
    page_lines->Clear();
    BuildPage((char)0);
    String path = settings->GetOutputPath();
    if (path.Length() > 0)
    {
        if (path[path.Length()] != '\\')
        {
            if (path[path.Length()] != '/')
                path = path + (char)92;
        }
    }
    path = path + "playerlist_exp.html";
    try
    {
        page_lines->SaveToFile(path);
    }
    catch (...)
    {
    }
}

void PlayerStats::ClearList()
{
    names->Clear();
    titles->Clear();
    levels->Clear();
    exps->Clear();
    genders->Clear();
    admin_levels->Clear();
    ranked_names->Clear();
    ranked_titles->Clear();
    ranked_levels->Clear();
    ranked_exps->Clear();
    ranked_genders->Clear();
    ranked_admin_levels->Clear();
}

// Adds a player to both lists: one sorted by name, the ranked one by experience, highest first.
void PlayerStats::AddPlayer(String name, String title, int experience, int level, int gender, int admin_level)
{
    int name_pos = 0;
    int rank_pos = 0;
    while (names->Count > name_pos)
    {
        if (!SortsAfter(name, names->Strings[name_pos]))
            break;
        name_pos++;
    }
    while (ranked_exps->Count > rank_pos)
    {
        if (StrToInt(ranked_exps->Strings[rank_pos]) < experience)
            break;
        rank_pos++;
    }
    names->Insert(name_pos, name);
    titles->Insert(name_pos, title);
    exps->Insert(name_pos, IntToStr(experience));
    levels->Insert(name_pos, IntToStr(level));
    genders->Insert(name_pos, IntToStr(gender));
    admin_levels->Insert(name_pos, IntToStr(admin_level));
    ranked_names->Insert(rank_pos, name);
    ranked_titles->Insert(rank_pos, title);
    ranked_exps->Insert(rank_pos, IntToStr(experience));
    ranked_levels->Insert(rank_pos, IntToStr(level));
    ranked_genders->Insert(rank_pos, IntToStr(gender));
    ranked_admin_levels->Insert(rank_pos, IntToStr(admin_level));
}

DEFINE_IS_LIST_MARKER(PlayerStats)

// Fills a tag of one list line for the player at list_index, from the name list ( by_name ) or the ranked one.
String PlayerStats::FillListLine(char by_name, String template_line)
{
    char in_tag;
    char changed;
    int tag_start;
    int tag_length;
    String line = template_line;
    in_tag = 0;
    changed = 0;
    tag_start = 0;
    tag_length = 0;
    for (int i = 1; i <= line.Length(); i++)
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
                    {
                        if (by_name)
                            line.Insert(names->Strings[list_index], tag_start);
                        else
                            line.Insert(ranked_names->Strings[list_index], tag_start);
                    }
                    if (tag == "{TITLE}")
                    {
                        if (by_name)
                            line.Insert(StripTags(titles->Strings[list_index]), tag_start);
                        else
                            line.Insert(StripTags(ranked_titles->Strings[list_index]), tag_start);
                    }
                    if (tag == "{LEVEL}")
                    {
                        if (by_name)
                            line.Insert(levels->Strings[list_index], tag_start);
                        else
                            line.Insert(ranked_levels->Strings[list_index], tag_start);
                    }
                    if (tag == "{EXP}")
                    {
                        if (by_name)
                            line.Insert(exps->Strings[list_index], tag_start);
                        else
                            line.Insert(ranked_exps->Strings[list_index], tag_start);
                    }
                    if (tag == "{GENDER}")
                    {
                        if (by_name)
                        {
                            if (genders->Strings[list_index] == "0")
                                line.Insert("Female", tag_start);
                            else
                                line.Insert("Male", tag_start);
                        }
                        else
                        {
                            if (ranked_genders->Strings[list_index] == "0")
                                line.Insert("Female", tag_start);
                            else
                                line.Insert("Male", tag_start);
                        }
                    }
                    // .. staff ( admin level 1 and up ) get the admin background
                    if (tag == "{BGCOLOR}")
                    {
                        if (by_name)
                        {
                            if (StrToInt(admin_levels->Strings[list_index]) > 0)
                                line.Insert("bgcolor='" + html_settings->GetBgColorAdmin() + "'", tag_start);
                        }
                        else
                        {
                            if (StrToInt(ranked_admin_levels->Strings[list_index]) > 0)
                                line.Insert("bgcolor='" + html_settings->GetBgColorAdmin() + "'", tag_start);
                        }
                    }
                    if (tag == "{FONTCOLOR}")
                    {
                        int admin_level = AdminLevel_Player;
                        if (by_name)
                            admin_level = StrToInt(admin_levels->Strings[list_index]);
                        else
                            admin_level = StrToInt(ranked_admin_levels->Strings[list_index]);
                        // admin level 1 shows in the player colour
                        if (admin_level <= 1)
                            line.Insert(html_settings->GetFontColorPlayer(), tag_start);
                        if (admin_level == AdminLevel_Guide)
                            line.Insert(html_settings->GetFontColorGuide(), tag_start);
                        if (admin_level == AdminLevel_Guardian)
                            line.Insert(html_settings->GetFontColorGuardian(), tag_start);
                        if (admin_level == AdminLevel_GameMaster)
                            line.Insert(html_settings->GetFontColorGM(), tag_start);
                        if (admin_level == AdminLevel_HighGameMaster)
                            line.Insert(html_settings->GetFontColorHGM(), tag_start);
                    }
                }
                i = line.Length();
                in_tag = 1;
            }
        }
    }
    return line;
}

// Builds the page: the list lines once for every player, and {COUNT} and {GENDATE} on the lines around them.
void PlayerStats::BuildPage(char by_name)
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
                    page_lines->Add(FillListLine(by_name, template_lines->Strings[j]));
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

DEFINE_STRIP_TAGS(PlayerStats)

// True when name sorts after other, or is the same, by byte value.
bool PlayerStats::SortsAfter(String name, String other)
{
    for (int i = 1; name.Length() >= i && other.Length() >= i; i++)
    {
        if ((unsigned char)name[i] < (unsigned char)other[i])
            return false;
        if ((unsigned char)name[i] > (unsigned char)other[i])
            return true;
    }
    if (name.Length() < other.Length())
        return false;
    return true;
}

PlayerStats::~PlayerStats()
{
}
