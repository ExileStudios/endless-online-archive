#include <vcl.h>
#pragma hdrstop

#include "Mapsign.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

MapSign::MapSign(int tile_x, int tile_y, String sign_text)
{
    x = tile_x;
    y = tile_y;
    text = sign_text;
}

MapSign::~MapSign()
{
}
