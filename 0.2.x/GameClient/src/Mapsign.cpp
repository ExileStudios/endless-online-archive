#include <vcl.h>
#pragma hdrstop

#include "Mapsign.h"

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
