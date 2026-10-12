#ifndef FilecacheH
#define FilecacheH

#include <Classes.hpp>
#include <SysUtils.hpp>
#include <ScktComp.hpp>
#include <DBTables.hpp>
#include <string.h>
#include <stdio.h>
#include <vector.h>
#include <deque.h>
#include <list.h>
#include <stack.h>
#include <queue.h>

struct Player;


struct FileCache;
struct TopGuild;
struct TopPlayer;

// The top players and the top guilds, kept between runs in cache/players.chk and cache/guilds.chk.
struct FileCache
{
    bool cache_ok;
    int accounts_count;
    int characters_count;
    int guilds_count;
    vector<TopPlayer *> top_players;
    vector<TopGuild *> top_guilds;
    TStringList *string_list;
    String current_line;
    int min_top_experience;

    FileCache();
    ~FileCache();
    void CheckCacheFile();
    void UpdatePlayerCache(Player *record);
    void LoadPlayerCache();
    void LoadGuildCache();
    String NextToken();
    void FlushCache();
};

struct TopGuild
{
    String ident_guild;
    String guild;
    int exptotal;
    int exphigh;
    int members;

    TopGuild();
    ~TopGuild();
};

struct TopPlayer
{
    int privilege;
    String name;
    String title;
    int level;
    int experience;
    int gender;

    TopPlayer();
    ~TopPlayer();
};


#endif
