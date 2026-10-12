#include <vcl.h>
#pragma hdrstop

#include "Mysqlthread.h"
#include "MainForm.h"
#include "Mysqltask.h"
#include "Packets.h"

#pragma package(smart_init)

__fastcall MySQLthread::MySQLthread(Dbtables::TSession *session_, Dbtables::TDatabase *database_,
    Dbtables::TQuery *query_, mySQLbuffer *queue_, bool CreateSuspended)
    : Classes::TThread(CreateSuspended)
{
    Priority = tpLower;
    session = session_;
    database = database_;
    query = query_;
    queue = queue_;
    FreeOnTerminate = 1;
}

// Takes the queued queries one at a time; with none left it suspends until the next one comes.
void __fastcall MySQLthread::Execute()
{
    while (!Terminated)
    {
        mySQLtask *job = 0;
        queue->lock->Acquire();
        if (queue->job_queue.size() > 0)
        {
            vector<mySQLtask *>::iterator it = queue->job_queue.begin();
            job = *it;
            queue->last_query_id = job->query_type;
            queue->last_player_id = job->player_id;
            queue->last_account_ident = job->expected_query_id;
            queue->job_queue.erase(it);
        }
        else
        {
            queue->last_player_id = -1;
            queue->last_account_ident = -1;
        }
        queue->lock->Release();
        if (job)
        {
            task = job;
            String query_text = task->query_text;
            if (query->Active)
                query->Active = false;
            query->SQL->Clear();
            query->SQL->Add(task->query_text);
            try
            {
                // .. below DbQueryType_FirstSelect a query only writes; the others select and are answered
                if (task->query_type < DbQueryType_FirstSelect)
                {
                    query->ExecSQL();
                    if (task->query_type != DbQueryType_ExecSilent)
                        Synchronize(OnResult);
                }
                else
                {
                    query->Active = true;
                    Synchronize(OnResult);
                }
            }
            catch (...)
            {
            }
        }
        if (!job)
            Suspend();
        else
        {
            delete job;
            Sleep(25);
        }
    }
}

// Runs in the main thread ( Synchronize ): hands the finished query to the server.
void __fastcall MySQLthread::OnResult()
{
    Packets *server = GUI->Mainform_GetServer();
    server->MysqlCallback_Dispatch(task);
}
