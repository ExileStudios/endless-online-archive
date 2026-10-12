#ifndef MsgboardH
#define MsgboardH

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



struct MsgBoard;

// A post on a message board.
struct MsgBoard
{
    short id;
    String poster;
    String subject;
    String message;

    MsgBoard();
    MsgBoard(int post_id, String post_poster, String post_subject, String post_message);
    ~MsgBoard();
};


#endif
