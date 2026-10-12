#include <vcl.h>
#pragma hdrstop

#include "Quest.h"

#pragma package(smart_init)

Quest::Quest(int quest_id)
{
    this->quest_id = quest_id;
    this->state_count = 0;
    this->loaded = 0;
}

Quest::~Quest()
{
}
