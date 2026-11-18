#include <vcl.h>
#pragma hdrstop

#include "Gfx_mapblends.h"
#include "Tmainform.h"
#include "Gfx_bank.h"
#include "Gfx_blend.h"

#pragma package(smart_init)

BlendGraphics::BlendGraphics(char preload)
{
    last_index = -1;
    pending_count = 0;
}

BlendGraphics::~BlendGraphics()
{
}

// On a map change: frees the blended overlay library while loads are pending and zeroes the pending count.
// A kept library is opened again.
void BlendGraphics::ReopenLibrary()
{
}

// Counts, per loaded blended overlay, the frames in a row in which GetGraphic did not fetch it.
void BlendGraphics::AgeGraphics()
{
    for (vector<BlendWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        if (!it->fetched && it->is_loaded)
            it->unfetched_frames++;
        else
            it->unfetched_frames = 0;
        it->fetched = false;
    }
}

// Loads one waiting blend graphic from its library each call; one that failed to load twice is freed and waits no
// longer.
void BlendGraphics::LoadPendingGraphic()
{
    vector<BlendWrapper>::iterator it;
    if (pending_count > 0)
    {
        library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_Shadows);
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
                    it->width = size.x;
                    it->height = size.y;
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

// Frees every blended overlay that has gone unfetched for the purge age.
void BlendGraphics::PurgeUnusedGraphics()
{
    bool result;
    // An erase ends the walk, so it starts over after each blended overlay it frees.
    do
    {
        result = false;
        for (vector<BlendWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
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

// Frees every blended overlay and empties the list.
void BlendGraphics::ReleaseGraphics()
{
    for (vector<BlendWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
        it->FreeResource();
    if (GetGraphicCount() >= 1)
    {
        entries.clear();
        last_index = -1;
        pending_count = 0;
    }
}

// Whether a blended overlay is loaded; the first request for it puts it in the queue.
bool BlendGraphics::RequestGraphic(int graphic_id)
{
    vector<BlendWrapper>::iterator it;
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
    // No blended overlay is queued while the game runs fullscreen in the background.
    if (not_queued && (Application->Active || !MainForm->config->GetFullscreen()))
        QueueGraphic(graphic_id);
    return is_loaded;
}

// The loaded blended overlay, marked as drawn so AgeGraphics does not count it.
BlendWrapper *BlendGraphics::GetGraphic(int graphic_id)
{
    vector<BlendWrapper>::iterator i;
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

void BlendGraphics::QueueGraphic(int graphic_id)
{
    BlendWrapper handle(graphic_id);
    entries.insert(entries.end(), handle);
    pending_count++;
}

unsigned int BlendGraphics::GetGraphicCount()
{
    return entries.size();
}
