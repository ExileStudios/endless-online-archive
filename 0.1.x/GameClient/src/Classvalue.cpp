#include <vcl.h>
#pragma hdrstop

#include "Classvalue.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

ClassValue::ClassValue(int record_id)
{
    class_id = record_id;
}

ClassValue::~ClassValue()
{
}
