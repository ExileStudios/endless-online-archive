#ifndef MysqlcontrolsH
#define MysqlcontrolsH

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

struct FileCache;
struct MySQLthread;
struct mySQLbuffer;


struct mySQLdb;

// The database: direct queries while the server starts, and the queue of queries the database thread runs.
struct mySQLdb
{
    FileCache *file_cache;
    TTimeStamp last_status_time;
    TTimeStamp connected_time;
    char pad_0x14[4];
    int query_error_count;
    int exec_error_count;
    mySQLbuffer *task_queue;
    MySQLthread *worker_thread;

    mySQLdb();
    ~mySQLdb();
    char TestConnection();
    void InitServerData(int version_major, int version_minor, int version_patch);
    void LoadCachedPlayers();
    void LoadCachedGuilds();
    void UpdateServerStatus(int refresh_seconds, int connections, int players, int most, String upload,
        String download);
    String Db_GetString(String label);
    unsigned int Db_GetInt(String label);
    void NextResultRecord();
    char ResultAtEnd();
    int GetResultCount();
    bool Mysql_SubmitQuery(int query_type, int player_id, int expected_query_id, String data, String query_text);
    bool Mysql_SubmitQuery_FromCallback(int query_type, int player_id, int expected_query_id, String data,
        String query_text);
    bool Query(String query);
    bool Mysql_SubmitExec(int account_ident, String query);
    bool Mysql_SubmitExec_FromCallback(int account_ident, String query);
    bool ExecDrop(String query);
    unsigned int Db_GetQueuedQueryCount();
    char Database_TryShutdown();
    char IsTaskPending(int account_ident);
    bool IsAsciiText(String value);
    bool IsAlphabeticText(String value);
    String Mysql_SanitizeString(String value, char uppercase);
    String Db_SanitizeString(String value);
    void NormalizePlayerText(String *message);
    TTimeStamp Server_GetStartTime();
    String DecodeString(String value);
};


#endif
