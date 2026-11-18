#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Gfx_bank.h"
#include "Gfx_tile.h"
#pragma package(smart_init)

TileGraphics::TileGraphics(char preload)
{
    last_index = -1;
    pending_count = 0;
    queue_drained = false;
    library_open = false;
    keep_library_open = preload;
    if (keep_library_open)
    {
        library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_MapTiles).c_str());
        library_open = true;
    }
}

TileGraphics::~TileGraphics()
{
    if (library_open || keep_library_open)
        FreeLibrary(library);
}

// On a map change: frees the ground tile library while loads are pending and zeroes the pending count.
// A kept library is opened again.
void TileGraphics::ReopenLibrary()
{
    if (pending_count > 0 || keep_library_open)
    {
        FreeLibrary(library);
        if (!keep_library_open)
            pending_count = 0;
    }
    if (keep_library_open)
        library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_MapTiles).c_str());
}

// Counts, per loaded ground tile, the frames in a row in which GetGraphic did not fetch it.
void TileGraphics::AgeGraphics()
{
    for (vector<TileWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        if (!it->was_used && it->is_loaded)
            it->idle_ticks++;
        else
            it->idle_ticks = 0;
        it->was_used = false;
    }
}

// Loads the first queued ground tile, opening the library when no load has it open.
void TileGraphics::LoadPendingGraphic()
{
    // The call after the ground tile queue empties closes the library again, unless it is kept open.
    if (queue_drained)
    {
        if (library_open && !keep_library_open)
        {
            FreeLibrary(library);
            library_open = false;
        }
        queue_drained = false;
    }
    if (pending_count > 0)
    {
        if (!library_open && !keep_library_open)
        {
            library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_MapTiles).c_str());
            library_open = true;
        }
        for (vector<TileWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
        {
            if (!it->is_loaded)
            {
                HMODULE open_library = library;
                it->CreateResource();
                GraphicSize size = it->resource->LoadSurface((int)open_library, it->graphic_id + 100);
                it->is_loaded = true;
                it->idle_ticks = 0;
                pending_count--;
                if (!pending_count)
                    queue_drained = true;
                break;
            }
        }
    }
}

// Frees every ground tile that has gone unfetched for the purge age.
bool TileGraphics::PurgeUnusedGraphics()
{
    bool result;
    // An erase ends the walk, so it starts over after each ground tile it frees.
    do
    {
        result = false;
        for (vector<TileWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
        {
            if (it->idle_ticks >= 16)
            {
                it->FreeResource();
                entries.erase(it);
                result = true;
                break;
            }
        }
    } while (result);
    return result;
}

// Frees every ground tile and empties the list.
void TileGraphics::ReleaseGraphics()
{
    int unused_int;
    for (vector<TileWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
        it->FreeResource();
    if (GetGraphicCount() >= 1)
    {
        entries.clear();
        last_index = -1;
        pending_count = 0;
    }
}

// Whether a ground tile is loaded; the first request for it puts it in the queue.
bool TileGraphics::RequestGraphic(int graphic_id)
{
    vector<TileWrapper>::iterator it;
    bool is_loaded = false;
    char not_queued = true;
    for (it = entries.begin(); it != entries.end(); it++)
    {
        if (graphic_id != it->graphic_id)
            continue;
        not_queued = false;
        if (it->is_loaded)
            is_loaded = true;
    }
    // No ground tile is queued while the game runs fullscreen in the background.
    if (not_queued && (Application->Active || !MainForm->config->GetFullscreen()))
        QueueGraphic(graphic_id);
    return is_loaded;
}

// The loaded ground tile, marked as drawn so AgeGraphics does not count it.
TileWrapper TileGraphics::GetGraphic(int graphic_id)
{
    vector<TileWrapper>::iterator i;
    for (i = entries.begin(); i != entries.end(); i++)
    {
        if (graphic_id == i->graphic_id && i->is_loaded)
        {
            i->was_used = true;
            break;
        }
    }
    return *i;
}

void TileGraphics::QueueGraphic(int graphic_id)
{
    TileWrapper handle(graphic_id);
    entries.insert(entries.end(), handle);
    pending_count++;
}

unsigned int TileGraphics::GetGraphicCount()
{
    return entries.size();
}
