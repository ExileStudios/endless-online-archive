#include <vcl.h>
#pragma hdrstop

#include "Mysqlcontrols.h"
#include "Filecache.h"
#include "MainForm.h"
#include "Mysqltask.h"
#include "Mysqlthread.h"
#include "Constants.h"

#pragma package(smart_init)

#define ENC_DB_PASSWORD_PARAM "5pz0agrc=wildhhzk"
#define ENC_DB_USERNAME_PARAM "xxz_hhvowmv=vnzmivhf"

// Creates the top lists' cache, the query queue and the database thread.
mySQLdb::mySQLdb()
{
    file_cache = new FileCache();
    task_queue = new mySQLbuffer();
    worker_thread = new MySQLthread(GUI->mysession, GUI->mysql, GUI->myquery, task_queue, false);
    query_error_count = 0;
    exec_error_count = 0;
    connected_time = DateTimeToTimeStamp(Now());
    last_status_time = DateTimeToTimeStamp(Now());
}

mySQLdb::~mySQLdb()
{
}

// Logs in to the database with its hidden user name and password; true when connected.
char mySQLdb::TestConnection()
{
    try
    {
        GUI->mysql->Params->Clear();
        GUI->mysql->Params->Add(DecodeString(ENC_DB_USERNAME_PARAM));
        GUI->mysql->Params->Add(DecodeString(ENC_DB_PASSWORD_PARAM));
        GUI->mysql->Connected = true;
    }
    catch (...)
    {
    }
    return GUI->mysql->Connected;
}

// The database at the start: the endl_server row, the table counts and the top 100 players and guilds.
void mySQLdb::InitServerData(int version_major, int version_minor, int version_patch)
{
    file_cache->CheckCacheFile();
    // .. without the last run's cache/cacheok.chk every character is set offline
    if (!file_cache->cache_ok)
        ExecDrop("UPDATE endl_characters SET online = 0 WHERE online = 1");
    ExecDrop("DELETE FROM endl_server");
    String connect_string = "INSERT INTO endl_server (version, characters, connections, players, uptime, upload, "
        "download) VALUES (";
    connect_string = connect_string + "'" + IntToStr(version_major) + "." + IntToStr(version_minor) + "." +
        IntToStr(version_patch) + "',0,0,0,0,'no data','no data')";
    ExecDrop(connect_string);
    // .. the number of accounts, characters and guilds, from the table status
    Query("SHOW TABLE STATUS FROM endless_db");
    while (!GUI->myquery->Eof)
    {
        if (GUI->myquery->Fields->Fields[0]->AsString == "endl_accounts")
        {
            try
            {
                if (LowerCase(GUI->myquery->Fields->Fields[3]->AsString) == "fixed")
                    file_cache->accounts_count = GUI->myquery->Fields->Fields[4]->AsInteger;
            }
            catch (...)
            {
            }
        }
        if (GUI->myquery->Fields->Fields[0]->AsString == "endl_characters")
        {
            try
            {
                if (LowerCase(GUI->myquery->Fields->Fields[3]->AsString) == "fixed")
                    file_cache->characters_count = GUI->myquery->Fields->Fields[4]->AsInteger;
            }
            catch (...)
            {
            }
        }
        if (GUI->myquery->Fields->Fields[0]->AsString == "endl_guilds")
        {
            try
            {
                if (LowerCase(GUI->myquery->Fields->Fields[3]->AsString) == "fixed")
                    file_cache->guilds_count = GUI->myquery->Fields->Fields[4]->AsInteger;
            }
            catch (...)
            {
            }
        }
        GUI->myquery->Next();
    }
    // .. the top 100 players and guilds: from the database, or from the cache the last run saved
    if (!file_cache->cache_ok)
    {
        Query("SELECT privilege, name, title, level, experience, gender FROM endl_characters WHERE privilege = 0 ORDER "
            "BY experience desc LIMIT 100");
        LoadCachedPlayers();
    }
    else
        file_cache->LoadPlayerCache();
    if (!file_cache->cache_ok)
    {
        Query("SELECT ident_guild, guild, SUM(level) as exptotal, MAX(level) as exphigh, COUNT(ident_guild) as members "
            "FROM endl_characters WHERE LENGTH(ident_guild) > 1 AND privilege = 0 GROUP BY ident_guild desc ORDER BY "
            "exptotal desc LIMIT 100");
        LoadCachedGuilds();
    }
    else
        file_cache->LoadGuildCache();
}

// Fills the top players from the query's rows.
void mySQLdb::LoadCachedPlayers()
{
    while (!GUI->myquery->Eof)
    {
        TopPlayer *entry = new TopPlayer();
        entry->privilege = GUI->myquery->Fields->FieldByName("privilege")->AsInteger;
        entry->name = GUI->myquery->Fields->FieldByName("name")->AsString;
        entry->title = GUI->myquery->Fields->FieldByName("title")->AsString;
        entry->level = GUI->myquery->Fields->FieldByName("level")->AsInteger;
        entry->experience = GUI->myquery->Fields->FieldByName("experience")->AsInteger;
        entry->gender = GUI->myquery->Fields->FieldByName("gender")->AsInteger;
        file_cache->top_players.insert(file_cache->top_players.end(), entry);
        GUI->myquery->Next();
    }
}

// Fills the top guilds from the query's rows.
void mySQLdb::LoadCachedGuilds()
{
    while (!GUI->myquery->Eof)
    {
        TopGuild *entry = new TopGuild();
        entry->ident_guild = GUI->myquery->Fields->FieldByName("ident_guild")->AsString;
        entry->guild = GUI->myquery->Fields->FieldByName("guild")->AsString;
        entry->exptotal = GUI->myquery->Fields->FieldByName("exptotal")->AsInteger;
        entry->exphigh = GUI->myquery->Fields->FieldByName("exphigh")->AsInteger;
        entry->members = GUI->myquery->Fields->FieldByName("members")->AsInteger;
        file_cache->top_guilds.insert(file_cache->top_guilds.end(), entry);
        GUI->myquery->Next();
    }
}

// Writes the server's numbers to endl_server, at most once every refresh_seconds.
void mySQLdb::UpdateServerStatus(int refresh_seconds, int connections, int players, int most, String upload,
    String download)
{
    TTimeStamp now_stamp;
    int date_diff;
    int time_diff;
    int elapsed;
    TTimeStamp stamp;
    now_stamp = DateTimeToTimeStamp(Now());
    date_diff = now_stamp.Date - last_status_time.Date;
    time_diff = now_stamp.Time - last_status_time.Time;
    elapsed = time_diff / MS_PER_SECOND + date_diff * SECONDS_PER_DAY;
    if (elapsed >= refresh_seconds)
    {
        last_status_time = DateTimeToTimeStamp(Now());
        stamp = DateTimeToTimeStamp(Now());
        int up_date_diff = stamp.Date - connected_time.Date;
        int up_time_diff = stamp.Time - connected_time.Time;
        int uptime = up_time_diff / 1800000 + up_date_diff * 48;  // in half hours
        String query = "UPDATE endl_server SET ";
        query = query + "uptime = " + IntToStr(uptime) + ",";
        query = query + "connections = " + IntToStr(connections) + ",";
        query = query + "players = " + IntToStr(players) + ",";
        query = query + "most = " + IntToStr(most) + ",";
        query = query + "upload = '" + upload + "',";
        query = query + "download = '" + download + "'";
        Mysql_SubmitExec(0, query);
    }
}

String mySQLdb::Db_GetString(String label)
{
    return GUI->myquery->Fields->FieldByName(label)->AsString;
}

unsigned int mySQLdb::Db_GetInt(String label)
{
    return GUI->myquery->Fields->FieldByName(label)->AsInteger;
}

void mySQLdb::NextResultRecord()
{
    GUI->myquery->Next();
}

char mySQLdb::ResultAtEnd()
{
    return GUI->myquery->Eof;
}

int mySQLdb::GetResultCount()
{
    return GUI->myquery->RecordCount;
}

// Queues a query for the database thread and wakes it; its result comes back to MysqlCallback_Dispatch.
bool mySQLdb::Mysql_SubmitQuery(int query_type, int player_id, int expected_query_id, String data, String query_text)
{
    mySQLtask *task = new mySQLtask(query_type, player_id, expected_query_id, data, query_text);
    task_queue->lock->Acquire();
    task_queue->EnqueueTask(task);
    worker_thread->Resume();
    task_queue->lock->Release();
    return true;
}

// Queues a query from a result callback, while the database thread is awake: no lock, no resume.
bool mySQLdb::Mysql_SubmitQuery_FromCallback(int query_type, int player_id, int expected_query_id, String data,
    String query_text)
{
    mySQLtask *task = new mySQLtask(query_type, player_id, expected_query_id, data, query_text);
    task_queue->EnqueueTask(task);
    return true;
}

// Runs a select at once, outside the database thread; false when it fails.
bool mySQLdb::Query(String query)
{
    bool ok = 1;
    try
    {
        if (GUI->myquery->Active)
            GUI->myquery->Active = false;
        GUI->myquery->SQL->Clear();
        GUI->myquery->SQL->Add(query);
        GUI->myquery->Active = true;
        if (GUI->myquery->RecordCount > 0)
        {
            if (GUI->myquery->Active)
                GUI->myquery->First();
        }
    }
    catch (...)
    {
        ok = 0;
        query_error_count++;
    }
    return ok;
}

// Queues a silent write for the database thread and wakes it.
bool mySQLdb::Mysql_SubmitExec(int account_ident, String query)
{
    mySQLtask *task = new mySQLtask(1, 0, account_ident, "", query);
    task_queue->lock->Acquire();
    task_queue->EnqueueTask(task);
    worker_thread->Resume();
    task_queue->lock->Release();
    return true;
}

// Queues a silent write from a result callback: no lock, no resume.
bool mySQLdb::Mysql_SubmitExec_FromCallback(int account_ident, String query)
{
    mySQLtask *task = new mySQLtask(1, 0, account_ident, "", query);
    task_queue->EnqueueTask(task);
    return true;
}

// Runs a write at once, outside the database thread; false when it fails.
bool mySQLdb::ExecDrop(String query)
{
    bool ok = 1;
    try
    {
        if (GUI->myquery->Active)
            GUI->myquery->Active = false;
        GUI->myquery->SQL->Clear();
        GUI->myquery->SQL->Add(query);
        GUI->myquery->ExecSQL();
    }
    catch (...)
    {
        ok = 0;
        exec_error_count++;
    }
    return ok;
}

// The number of queries waiting ( the panel's "sql" count ).
unsigned int mySQLdb::Db_GetQueuedQueryCount()
{
    return task_queue->job_queue.size();
}

// True when the database thread is idle and suspended; it is then told to end.
char mySQLdb::Database_TryShutdown()
{
    if (task_queue->job_queue.size() > 0)
        return 0;
    else if (task_queue->last_player_id > 0)
        return 0;
    else if (worker_thread->Suspended)
    {
        worker_thread->Terminate();
        return 1;
    }
}

char mySQLdb::IsTaskPending(int account_ident)
{
    return task_queue->HasPendingTask(account_ident);
}

// True when no character is past 'z'.
bool mySQLdb::IsAsciiText(String value)
{
    for (int i = 1; i <= value.Length(); i++)
    {
        if ((unsigned char)value[i] > 122)
            return false;
    }
    return true;
}

// True when the text is letters only, A to Z and a to z.
bool mySQLdb::IsAlphabeticText(String value)
{
    bool result = 1;
    if (value.Length() >= 1)
    {
        int i = value.Length();
        while (i >= 1)
        {
            if ((unsigned char)value[i] < 65 || (unsigned char)value[i] > 122)
            {
                result = 0;
                break;
            }
            if ((unsigned char)value[i] > 90)
            {
                if ((unsigned char)value[i] < 97)
                {
                    result = 0;
                    break;
                }
            }
            i--;
        }
    }
    return result;
}

// The text with only its letters and spaces left, in lower or upper case.
String mySQLdb::Mysql_SanitizeString(String value, char uppercase)
{
    if (value.Length() >= 1)
    {
        for (int i = value.Length(); i >= 1; i--)
        {
            if ((unsigned char)value[i] == 32)
                continue;
            if ((unsigned char)value[i] >= 91 && (unsigned char)value[i] <= 96)
                value.Delete(i, 1);
            else if ((unsigned char)value[i] <= 64 || (unsigned char)value[i] >= 123)
                value.Delete(i, 1);
        }
    }
    if (!uppercase)
        return LowerCase(value);
    else
        return UpperCase(value);
}

// The text in lower case, without the characters a query could misread ( quotes, brackets, ; = / and the like ).
String mySQLdb::Db_SanitizeString(String value)
{
    if (value.Length() >= 1)
    {
        for (int i = value.Length(); i >= 1; i--)
        {
            if (String(value[i]) == "'" || value[i] == 37 || value[i] == 91 || value[i] == 42 || value[i] == 93 ||
                value[i] == 40 || value[i] == 44 || value[i] == 41 || value[i] == 38 || value[i] == 123 ||
                value[i] == 35 || value[i] == 125 || value[i] == 34 || value[i] == 58 || value[i] == 59 ||
                value[i] == -56 || value[i] == 61 || value[i] == 47 || value[i] == 92 || value[i] == -1)
                value.Delete(i, 1);
        }
    }
    return LowerCase(value);
}

// Tames a long chat line ( 60 and up ): 128 characters at most, 20 symbols at most, lower case past 40 capitals.
void mySQLdb::NormalizePlayerText(String *message)
{
    if ((*message).Length() < 60)
        return;
    if ((*message).Length() > 128)
        (*message) = (*message).SubString(1, 128);
    int caps = 0;
    int other = 0;
    for (int i = (*message).Length(); i >= 1; i--)
    {
        if ((unsigned char)(*message)[i] >= 65 && (unsigned char)(*message)[i] <= 90)
            caps++;
        if ((unsigned char)(*message)[i] >= 65)
        {
            if ((unsigned char)(*message)[i] <= 122)
            {
                // a 'W' counts as a symbol
                if ((unsigned char)(*message)[i] != 87)
                    continue;
            }
        }
        other++;
        if (other > 20)
            (*message).Delete(i, 1);
    }
    if (caps > 40)
        (*message) = LowerCase(*message);
}

// When the server started.
TTimeStamp mySQLdb::Server_GetStartTime()
{
    return connected_time;
}

// Decodes a hidden string: reversed, then the digits and the lower case letters mirrored.
String mySQLdb::DecodeString(String value)
{
    String reversed = "";
    String result = "";
    try
    {
        for (int i = value.Length(); i >= 1; i--)
            reversed = reversed + value[i];
        for (int j = 1; j <= value.Length(); j++)
        {
            unsigned char letter = reversed[j];
            unsigned char ch = letter;
            int code = ch;
            char done = 0;
            if (!done && code >= 48 && code <= 57)
            {
                code = 57 - code + 48;
                ch = code;
                letter = ch;
                result.Insert(String((char)letter), result.Length() + 1);
                done = 1;
            }
            if (!done && code >= 97 && code <= 122)
            {
                code = 122 - code + 97;
                ch = code;
                letter = ch;
                result.Insert(String((char)letter), result.Length() + 1);
                done = 1;
            }
            if (!done)
                result.Insert(String((char)letter), result.Length() + 1);
        }
    }
    catch (...)
    {
    }
    return result;
}
