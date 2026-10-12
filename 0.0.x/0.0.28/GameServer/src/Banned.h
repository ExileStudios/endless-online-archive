#ifndef BannedH
#define BannedH

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

struct mySQLdb;

struct Asocketban;
struct Banned;

// One ban: an ip ( a part may be * ), an hdid, its type, and how many seconds it lasts from ban_date.
struct Asocketban
{
    String hdid;
    short octet[4];
    char ban_type;
    int duration;
    TTimeStamp ban_date;
};

// The bans: those of endl_banlist and those the admins add; an ip or an hdid can match.
struct Banned
{
    mySQLdb *db_handle;
    TList *ban_list;
    int ban_time;
    unsigned char ban_type;

    Banned(mySQLdb *db_handle);
    ~Banned();
    void AddBan(String ip, String serial, bool permanent, int duration);
    void AddBan(String ip, String hdid, char ban_type, int duration);
    char IsBanned(String ip, String hdid);
    int GetBanType();
    int GetBanTime();
};

#endif
