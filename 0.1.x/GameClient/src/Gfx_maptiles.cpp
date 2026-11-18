#include <vcl.h>
#pragma hdrstop

#include "Gfx_maptiles.h"
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
        if (!it->fetched && it->is_loaded)
            it->unfetched_frames++;
        else
            it->unfetched_frames = 0;
        it->fetched = false;
    }
}

// Loads one waiting ground tile from its library each call; one that failed to load twice is freed and waits no
// longer.
void TileGraphics::LoadPendingGraphic()
{
    vector<TileWrapper>::iterator it;
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
        it = entries.begin();
        while (it != entries.end())
        {
            if (!it->is_loaded)
            {
                if (it->idle_ticks > 1)
                {
                    if (it->idle_ticks == 2)
                    {
                        it->idle_ticks = 250;
                        it->FreeResource();
                        pending_count--;
                        if (!pending_count)
                            queue_drained = true;
                    }
                }
                else
                {
                    int open_library = (int)library;
                    it->CreateResource();
                    GraphicSize size = it->resource->LoadSurface(open_library, it->graphic_id + 100);
                    if (size.width == -1)
                    {
                        it->idle_ticks++;
                        break;
                    }
                    it->is_loaded = true;
                    it->unfetched_frames = 0;
                    it->idle_ticks = 0;
                    pending_count--;
                    if (!pending_count)
                        queue_drained = true;
                    break;
                }
            }
            it++;
        }
    }
}

// Frees every ground tile that has gone unfetched for the purge age.
void TileGraphics::PurgeUnusedGraphics()
{
    bool result;
    // An erase ends the walk, so it starts over after each ground tile it frees.
    do
    {
        result = false;
        for (vector<TileWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
        {
            if (it->unfetched_frames >= 100)
            {
                it->FreeResource();
                entries.erase(it);
                result = true;
                break;
            }
        }
    } while (result);
}

// Frees every ground tile and empties the list.
void TileGraphics::ReleaseGraphics()
{
    for (vector<TileWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
        it->FreeResource();
    if (GetGraphicCount() >= 1)
    {
        entries.clear();
        last_index = -1;
        if (pending_count > 0)
        {
            pending_count = 0;
            queue_drained = true;
        }
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
        if (graphic_id == it->graphic_id)
        {
            not_queued = false;
            if (it->is_loaded)
                is_loaded = true;
        }
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
            i->fetched = true;
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
