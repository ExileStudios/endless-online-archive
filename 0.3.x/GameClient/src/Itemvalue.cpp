#include <vcl.h>
#pragma hdrstop

#include "Itemvalue.h"

#pragma package(smart_init)

ItemValue::ItemValue()
{
}

ItemValue::ItemValue(int record_id)
{
    item_id = record_id;
}

ItemValue::~ItemValue()
{
}
