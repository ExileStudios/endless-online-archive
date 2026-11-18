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
}

TileGraphics::~TileGraphics()
{
}

// On a map change: frees the ground tile library while loads are pending and zeroes the pending count.
// A kept library is opened again.
void TileGraphics::ReopenLibrary()
{
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
    if (pending_count > 0)
    {
        library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MapTiles);
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
                    }
                }
                else
                {
                    int open_library = library;
                    it->CreateResource();
                    TPoint size = it->resource->LoadSurface(open_library, it->graphic_id + 100);
                    if (size.x == -1)
                        it->idle_ticks++;
                    else
                    {
                        it->is_loaded = true;
                        it->unfetched_frames = 0;
                        it->idle_ticks = 0;
                        pending_count--;
                    }
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
            if (it->unfetched_frames >= 1000)
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
TileWrapper *TileGraphics::GetGraphic(int graphic_id)
{
    vector<TileWrapper>::iterator i;
    for (i = entries.begin(); i != entries.end(); i++)
    {
        if (graphic_id == i->graphic_id && i->is_loaded)
        {
            i->fetched = true;
            return i;
        }
    }
    return 0;
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
