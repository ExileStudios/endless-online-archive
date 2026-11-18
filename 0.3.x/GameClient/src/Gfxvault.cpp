#include <vcl.h>
#pragma hdrstop

#include "Gfxvault.h"
#include "Tmainform.h"

#pragma package(smart_init)

GfxVault::GfxVault()
{
    for (int i = 0; i < 26; i++)
    {
        file_names[i] = "";
        is_open[i] = false;
        keep_open[i] = false;
        libraries[i] = 0;
        unused_values[i] = -1;
    }
    if (MainForm->config->GetPreloadCharacterGfx())
    {
        for (int j = GfxLibrary_FirstPaperdoll; j <= GfxLibrary_LastPaperdoll; j++)
            keep_open[j] = true;
    }
    if (MainForm->config->GetPreloadMapGfx())
    {
        keep_open[GfxLibrary_MapObjects] = true;
        keep_open[GfxLibrary_MapWalls] = true;
    }
    if (MainForm->config->GetPreloadTileGfx())
    {
        keep_open[GfxLibrary_MapTiles] = true;
        keep_open[GfxLibrary_MapTops] = true;
    }
}

GfxVault::~GfxVault()
{
}

// The handle of the numbered graphics file, loading it as a resource library when it is not open yet; 0 when it
// cannot be loaded.
int GfxVault::OpenLibrary(int file_number)
{
    if (is_open[file_number])
    {
        idle_ticks[file_number] += 2;
        if (idle_ticks[file_number] > 6)
            idle_ticks[file_number] = 6;
        return GetLibrary(file_number);
    }
    if (libraries[file_number])
        CloseLibrary(file_number);
    String path = GetGfxFileName(file_number);
    libraries[file_number] = LoadLibraryExA(path.c_str(), 0, 35);
    if (!libraries[file_number])
    {
        is_open[file_number] = false;
        return 0;
    }
    idle_ticks[file_number] = 6;
    is_open[file_number] = true;
    return (int)libraries[file_number];
}

// The file name kept for the library, or its usual path when none is.
String GfxVault::GetGfxFileName(int file_number)
{
    if (file_names[file_number].Length() > 4)
        return file_names[file_number];
    return MainForm->files->GetGfxPath(file_number);
}

void GfxVault::SetGfxFileName(int file_number, String path)
{
    file_names[file_number] = path;
}

int GfxVault::GetLibrary(int file_number)
{
    if (is_open[file_number])
        return (int)libraries[file_number];
    return 0;
}

// Counts down each open library's idle time, closing it when that runs out.
void GfxVault::AgeLibraries()
{
    for (int i = 0; i < 26; i++)
    {
        if (is_open[i] && idle_ticks[i] > 0 && !keep_open[i])
        {
            idle_ticks[i]--;
            if (idle_ticks[i] <= 0)
                CloseLibrary(i);
        }
    }
}

// Frees the library's module and marks it closed.
void GfxVault::CloseLibrary(int file_number)
{
    if (libraries[file_number])
        FreeLibrary(libraries[file_number]);
    is_open[file_number] = false;
    libraries[file_number] = 0;
}

// Closes all 28 libraries.
void GfxVault::CloseAllLibraries()
{
    for (int i = 0; i < 26; i++)
        CloseLibrary(i);
}
