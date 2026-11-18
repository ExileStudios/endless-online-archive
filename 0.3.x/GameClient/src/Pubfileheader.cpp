#include <vcl.h>
#pragma hdrstop

#include "Pubfileheader.h"

#pragma package(smart_init)

PubfileHeader::PubfileHeader(String name)
{
    file_name = name;
    load_state = 2;
    records_read = 0;
}

PubfileHeader::~PubfileHeader()
{
}
