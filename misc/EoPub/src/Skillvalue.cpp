#include <vcl.h>
#pragma hdrstop

#include "Skillvalue.h"

#pragma package(smart_init)

SkillValue::SkillValue()
{
}

SkillValue::SkillValue(int record_id)
{
    skill_id = record_id;
}

SkillValue::~SkillValue()
{
}
