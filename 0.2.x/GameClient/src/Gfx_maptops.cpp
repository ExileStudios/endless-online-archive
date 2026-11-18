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
    queue_drained = false;
    library_open = false;
    keep_library_open = preload;
    if (keep_library_open)
    {
        library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_MapTops).c_str());
        library_open = true;
    }
}

TopGraphics::~TopGraphics()
{
    if (library_open || keep_library_open)
        FreeLibrary(library);
}

// On a map change: frees the top layer graphic library while loads are pending and zeroes the pending count.
// A kept library is opened again.
void TopGraphics::ReopenLibrary()
{
    if (pending_count > 0 || keep_library_open)
    {
        FreeLibrary(library);
        if (!keep_library_open)
            pending_count = 0;
    }
    if (keep_library_open)
        library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_MapTops).c_str());
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
    // The call after the top layer graphic queue empties closes the library again, unless it is kept open.
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
            library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_MapTops).c_str());
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

// Frees every top layer graphic that has gone unfetched for the purge age.
void TopGraphics::PurgeUnusedGraphics()
{
    bool result;
    do
    {
        result = false;
        for (vector<TopWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
        {
            if (it->unfetched_frames >= 16)
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
        if (pending_count > 0)
        {
            pending_count = 0;
            queue_drained = true;
        }
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
TopWrapper TopGraphics::GetGraphic(int graphic_id)
{
    vector<TopWrapper>::iterator i;
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
