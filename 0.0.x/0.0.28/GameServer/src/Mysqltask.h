#ifndef MysqltaskH
#define MysqltaskH

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
#include <Syncobjs.hpp>

enum DbQueryType
{
    DbQueryType_ExecSilent = 1,
    DbQueryType_FirstSelect = 60,
    DbQueryType_Login = 64,
    DbQueryType_LoginCharacters = 65,
    DbQueryType_AccountChangePassword = 66,
    DbQueryType_AccountCheck = 67,
    DbQueryType_AccountCreate = 68,
    DbQueryType_CharacterCreate = 69,
    DbQueryType_CharacterList = 70,
    DbQueryType_GuildRankName = 71,
    DbQueryType_GuildRankTarget = 72,
    DbQueryType_GuildKickMember = 73,
    DbQueryType_GuildDescription = 74,
    DbQueryType_GuildRanks = 75,
    DbQueryType_GuildBank = 76,
    DbQueryType_GuildMemberList = 77,
    DbQueryType_GuildInfo = 78,
    DbQueryType_GuildRankedMembers = 79,
    DbQueryType_GuildCreateCheck = 80,
    DbQueryType_GuildRequestCheck = 81,
    DbQueryType_GuildJoin = 82,
    DbQueryType_GuildTopList = 83
};



struct mySQLbuffer;
struct mySQLtask;

// The queue of queries for the database thread; the server and the thread share it under a critical section.
struct mySQLbuffer
{
    TCriticalSection *lock;
    vector<mySQLtask *> job_queue;
    int last_player_id;
    int last_account_ident;
    int last_query_id;

    mySQLbuffer();
    ~mySQLbuffer();
    void EnqueueTask(mySQLtask *task);
    bool HasPendingTask(int account_ident);
};

// One query for the database thread, and what to answer when it is done.
struct mySQLtask
{
    int query_type;
    int player_id;
    int expected_query_id;
    String data;
    String query_text;

    mySQLtask(int query_type_, int player_id_, int expected_query_id_, String data_, String query_text_);
    ~mySQLtask();
};


#endif
