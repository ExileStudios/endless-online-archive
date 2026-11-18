#include <vcl.h>
#pragma hdrstop

#include "Gfx_barber.h"
#include "Tmainform.h"
#include "Gfx_bank.h"
#include <TDx_Draw_Library_Install.H>
#pragma package(smart_init)

BarberGraphics::BarberGraphics()
{
    surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
}

BarberGraphics::~BarberGraphics()
{
    surface->Destroy();
    delete surface;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    color_key->Clear();
    delete color_key;
}

// Loads the barber window background from the library into a surface sized to the bitmap; returns its size,
// or -1 when the bitmap could not be drawn onto the barber window surface.
DEFINE_BANK_LOAD_SURFACE(BarberGraphics)
