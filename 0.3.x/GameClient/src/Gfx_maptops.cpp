#include <vcl.h>
#pragma hdrstop

#include "Gfx_maptops.h"
#include "Tmainform.h"
#include "Gfx_bank.h"
#include "Gfx_top.h"

#pragma package(smart_init)

TopGraphics::TopGraphics(char preload)
{
    last_index = -1;
    pending_count = 0;
}

TopGraphics::~TopGraphics()
{
}

// On a map change: frees the top layer graphic library while loads are pending and zeroes the pending count.
// A kept library is opened again.
void TopGraphics::ReopenLibrary()
{
}

// Counts, per loaded top layer graphic, the frames in a row in which GetGraphic did not fetch it.
void TopGraphics::AgeGraphics()
{
    for (vector<TopWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        if (!it->fetched && it->is_loaded)
            it->unfetched_frames++;
        else
            it->unfetched_frames = 0;
        it->fetched = false;
    }
}

// Loads one waiting top layer graphic from its library each call; one that failed to load twice is freed and waits
// no longer.
void TopGraphics::LoadPendingGraphic()
{
    vector<TopWrapper>::iterator it;
    if (pending_count > 0)
    {
        library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MapTops);
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

// Frees and removes every top layer graphic unused for 500 ticks or more.
void TopGraphics::PurgeUnusedGraphics()
{
    bool result;
    // An erase ends the walk, so it starts over after each top layer graphic it frees.
    do
    {
        result = false;
        for (vector<TopWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
        {
            if (it->unfetched_frames >= 500)
            {
                it->FreeResource();
                entries.erase(it);
                result = true;
                break;
            }
        }
    } while (result);
}

// Frees every top layer graphic and empties the list.
void TopGraphics::ReleaseGraphics()
{
    for (vector<TopWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
        it->FreeResource();
    if (GetGraphicCount() >= 1)
    {
        entries.clear();
        last_index = -1;
        pending_count = 0;
    }
}

// Whether a top layer graphic is loaded; the first request for it puts it in the queue.
bool TopGraphics::RequestGraphic(int graphic_id)
{
    vector<TopWrapper>::iterator it;
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
    // No top layer graphic is queued while the game runs fullscreen in the background.
    if (not_queued && (Application->Active || !MainForm->config->GetFullscreen()))
        QueueGraphic(graphic_id);
    return is_loaded;
}

// The loaded top layer graphic, marked as drawn so AgeGraphics does not count it.
TopWrapper *TopGraphics::GetGraphic(int graphic_id)
{
    vector<TopWrapper>::iterator i;
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

void TopGraphics::QueueGraphic(int graphic_id)
{
    TopWrapper handle(graphic_id);
    entries.insert(entries.end(), handle);
    pending_count++;
}

unsigned int TopGraphics::GetGraphicCount()
{
    return entries.size();
}
