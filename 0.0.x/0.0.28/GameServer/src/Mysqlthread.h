#ifndef MysqlthreadH
#define MysqlthreadH

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

struct mySQLbuffer;
struct mySQLtask;


struct MySQLthread;

// The database thread: runs the queued queries one by one and hands each result to the server.
struct MySQLthread : public Classes::TThread
{
    TSession *session;
    TDatabase *database;
    TQuery *query;
    char pad_0x3c[4];
    mySQLtask *task;
    mySQLbuffer *queue;

    __fastcall MySQLthread(Dbtables::TSession *session_, Dbtables::TDatabase *database_, Dbtables::TQuery *query_,
        mySQLbuffer *queue_, bool CreateSuspended);
    void __fastcall Execute();
    void __fastcall OnResult();
};


#endif
