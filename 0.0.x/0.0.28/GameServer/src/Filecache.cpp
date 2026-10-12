#include <vcl.h>
#pragma hdrstop

#include "Filecache.h"
#include "Player.h"

#pragma package(smart_init)

#include <io.h>
#include <stdio.h>

FileCache::FileCache()
{
    cache_ok = false;
    accounts_count = 0;
    characters_count = 0;
    guilds_count = 0;
    min_top_experience = 0;
    string_list = new TStringList();
}

FileCache::~FileCache()
{
}

// Notes whether cache/cacheok.chk is there ( the last run saved its cache ), and removes it.
void FileCache::CheckCacheFile()
{
    String path = "./cache/cacheok.chk";
    if (!access(path.c_str(), 0))
    {
        cache_ok = true;
        remove(path.c_str());
    }
}

// Puts a player in the top list when they pass its lowest experience; admins are left out.
void FileCache::UpdatePlayerCache(Player *record)
{
    vector<TopPlayer *>::iterator it;
    if (min_top_experience < record->experience)
    {
        if (!record->admin_level)
        {
            TopPlayer *last = 0;
            char found = 0;
            int min_experience = 2147483640;
            for (it = top_players.begin(); it != top_players.end(); it++)
            {
                if ((*it)->name == record->name)
                {
                    (*it)->experience = record->experience;
                    (*it)->title = record->title;
                    (*it)->level = record->level;
                    (*it)->gender = record->gender;
                    found = 1;
                }
                if ((*it)->experience < min_experience)
                {
                    min_experience = (*it)->experience;
                    last = *it;
                }
            }
            if (!found && last)
            {
                if (last->experience < record->experience)
                {
                    last->name = record->name;
                    last->experience = record->experience;
                    last->title = record->title;
                    last->level = record->level;
                    last->gender = record->gender;
                }
                min_experience = 2147483640;
                for (it = top_players.begin(); it != top_players.end(); it++)
                {
                    if ((*it)->experience < min_experience)
                        min_experience = (*it)->experience;
                }
            }
            min_top_experience = min_experience;
        }
    }
}

// Reads cache/players.chk, a line a player: privilege, name, title, level, experience, gender.
void FileCache::LoadPlayerCache()
{
    string_list->Clear();
    try
    {
        string_list->LoadFromFile("./cache/players.chk");
    }
    catch (...)
    {
    }
    for (int i = 0; i < string_list->Count; i++)
    {
        current_line = string_list->Strings[i];
        TopPlayer *entry = new TopPlayer();
        entry->privilege = StrToInt(NextToken());
        entry->name = NextToken();
        entry->title = NextToken();
        entry->level = StrToInt(NextToken());
        entry->experience = StrToInt(NextToken());
        entry->gender = StrToInt(NextToken());
        top_players.insert(top_players.end(), entry);
    }
}

// Reads cache/guilds.chk, a line a guild: ident, name, experience, members.
void FileCache::LoadGuildCache()
{
    string_list->Clear();
    try
    {
        string_list->LoadFromFile("./cache/guilds.chk");
    }
    catch (...)
    {
    }
    for (int i = 0; i < string_list->Count; i++)
    {
        current_line = string_list->Strings[i];
        TopGuild *entry = new TopGuild();
        entry->ident_guild = NextToken();
        entry->guild = NextToken();
        entry->exptotal = StrToInt(NextToken());
        entry->members = StrToInt(NextToken());
        top_guilds.insert(top_guilds.end(), entry);
    }
}

// The next `;` separated field of the line being read.
String FileCache::NextToken()
{
    int pos = current_line.Pos(";");
    if (pos < 1)
        return current_line;
    String result = current_line.SubString(1, pos - 1);
    current_line.Delete(1, pos);
    return result;
}

// Writes both top lists back ( only one with 100 entries or more ), then cache/cacheok.chk.
void FileCache::FlushCache()
{
    string_list->Clear();
    if (top_players.size() > 99)
    {
        for (vector<TopPlayer *>::iterator it = top_players.begin(); it != top_players.end(); it++)
        {
            String player_line = IntToStr((*it)->privilege) + ";";
            player_line = player_line + (*it)->name + ";";
            player_line = player_line + (*it)->title + ";";
            player_line = player_line + IntToStr((*it)->level) + ";";
            player_line = player_line + IntToStr((*it)->experience) + ";";
            player_line = player_line + IntToStr((*it)->gender) + ";";
            string_list->Add(player_line);
        }
        string_list->SaveToFile("./cache/players.chk");
    }
    string_list->Clear();
    if (top_guilds.size() > 99)
    {
        for (vector<TopGuild *>::iterator it = top_guilds.begin(); it != top_guilds.end(); it++)
        {
            String guild_line = (*it)->ident_guild + ";";
            guild_line = guild_line + (*it)->guild + ";";
            guild_line = guild_line + IntToStr((*it)->exptotal) + ";";
            guild_line = guild_line + IntToStr((*it)->members) + ";";
            string_list->Add(guild_line);
        }
        string_list->SaveToFile("./cache/guilds.chk");
    }
    FILE *fp = fopen("./cache/cacheok.chk", "wb");
    fclose(fp);
}

TopPlayer::TopPlayer()
{
}

TopPlayer::~TopPlayer()
{
}

TopGuild::TopGuild()
{
}

TopGuild::~TopGuild()
{
}
