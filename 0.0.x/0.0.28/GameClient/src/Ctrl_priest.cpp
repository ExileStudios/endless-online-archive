#include <vcl.h>
#pragma hdrstop

#include "Ctrl_priest.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

PriestController::PriestController()
{
}

PriestController::~PriestController()
{
}

int *Ctrl_priest_SetRequestId(int *priest, int request_id)
{
    *priest = request_id;
    return priest;
}

int PriestController::GetRequestId()
{
    return request_id;
}
