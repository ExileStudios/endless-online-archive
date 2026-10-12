#include <vcl.h>
#pragma hdrstop

#include "Mysqltask.h"

#pragma package(smart_init)

mySQLtask::mySQLtask(int query_type_, int player_id_, int expected_query_id_, String data_, String query_text_)
{
    query_type = query_type_;
    player_id = player_id_;
    expected_query_id = expected_query_id_;
    data = data_;
    query_text = query_text_;
}

mySQLtask::~mySQLtask()
{
}

mySQLbuffer::mySQLbuffer()
{
    lock = new TCriticalSection();
    last_player_id = -1;
    last_account_ident = -1;
}

mySQLbuffer::~mySQLbuffer()
{
    delete lock;
}

// Queues a query; one the player already has queued or running is dropped, unless it is a silent write.
void mySQLbuffer::EnqueueTask(mySQLtask *task)
{
    if (task->player_id != last_player_id || task->query_type != last_query_id ||
        task->query_type == DbQueryType_ExecSilent)
    {
        for (vector<mySQLtask *>::iterator it = job_queue.begin(); it != job_queue.end(); it++)
        {
            if ((*it)->player_id == task->player_id && task->query_type != DbQueryType_ExecSilent)
            {
                if ((*it)->query_type == task->query_type)
                    return;
            }
        }
        job_queue.insert(job_queue.end(), task);
    }
}

// True while a silent write for this account is queued or running.
bool mySQLbuffer::HasPendingTask(int account_ident)
{
    for (vector<mySQLtask *>::iterator it = job_queue.begin(); it != job_queue.end(); it++)
    {
        if ((*it)->expected_query_id == account_ident && (*it)->query_type == DbQueryType_ExecSilent)
            return true;
    }
    if (last_account_ident == account_ident && last_query_id == 1)
        return true;
    return false;
}
