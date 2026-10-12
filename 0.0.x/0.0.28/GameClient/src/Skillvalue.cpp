#include <vcl.h>
#pragma hdrstop

#include "Skillvalue.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

SkillValue::SkillValue(int record_id)
{
    skill_id = record_id;
}

SkillValue::~SkillValue()
{
}
