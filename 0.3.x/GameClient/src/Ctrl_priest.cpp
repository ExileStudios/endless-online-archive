#include <vcl.h>
#pragma hdrstop

#include "Ctrl_priest.h"

#pragma package(smart_init)

PriestController::PriestController()
{
}

PriestController::~PriestController()
{
}

void PriestController::SetRequestId(int id)
{
    request_id = id;
}

int PriestController::GetRequestId()
{
    return request_id;
}
