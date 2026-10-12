#include <vcl.h>
#pragma hdrstop

#include "Sys_msg.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

Msg::Msg(int post_number, String post_author, String post_subject)
{
    post_id = post_number;
    author = post_author;
    subject = post_subject;
    body = "";
}

Msg::~Msg()
{
}
