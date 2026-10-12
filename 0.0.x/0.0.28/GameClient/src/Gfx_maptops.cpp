#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Gfx_bank.h"
#include "Gfx_top.h"
#pragma package(smart_init)

TopGraphics::TopGraphics(char preload)
{
    last_index = -1;
    pending_count = 0;
    queue_drained = 0;
    library_open = 0;
    keep_library_open = preload;
    if (keep_library_open)
    {
        library = LoadLibraryA(MainForm->strings->GetGfxPath(7).c_str());
        library_open = 1;
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
        library = LoadLibraryA(MainForm->strings->GetGfxPath(7).c_str());
}

// Counts, per loaded top layer graphic, the frames in a row in which GetGraphic did not fetch it.
void TopGraphics::AgeGraphics()
{
    for (vector<TopWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        if (!it->was_used && it->is_loaded)
            it->idle_ticks++;
        else
            it->idle_ticks = 0;
        it->was_used = 0;
    }
}

// Loads the first queued top layer graphic, opening the library when no load has it open.
void TopGraphics::LoadPendingGraphic()
{
    // The call after the top layer graphic queue empties closes the library again, unless it is kept open.
    if (queue_drained)
    {
        if (library_open && !keep_library_open)
        {
            FreeLibrary(library);
            library_open = 0;
        }
        queue_drained = 0;
    }
    if (pending_count > 0)
    {
        if (!library_open && !keep_library_open)
        {
            library = LoadLibraryA(MainForm->strings->GetGfxPath(7).c_str());
            library_open = 1;
        }
        for (vector<TopWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
        {
            if (!it->is_loaded)
            {
                HMODULE open_library = library;
                it->CreateResource();
                GraphicSize size = it->resource->LoadSurface((int)open_library, it->graphic_id + 100);
                if (size.width != -1 && size.height != 1)
                {
                    it->is_loaded = 1;
                    it->idle_ticks = 0;
                    pending_count--;
                }
                if (!pending_count)
                    queue_drained = 1;
                break;
            }
        }
    }
}

// Frees every top layer graphic that has gone unfetched for the purge age.
bool TopGraphics::PurgeUnusedGraphics()
{
    bool result;
    // An erase ends the walk, so it starts over after each top layer graphic it frees.
    do
    {
        result = 0;
        for (vector<TopWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
        {
            if (it->idle_ticks >= 16)
            {
                it->FreeResource();
                entries.erase(it);
                result = 1;
                break;
            }
        }
    } while (result);
    return result;
}

// Frees every top layer graphic and empties the list.
void TopGraphics::ReleaseGraphics()
{
    int __slot1;
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
    bool is_loaded = 0;
    char not_queued = 1;
    vector<TopWrapper>::iterator it;
    for (it = entries.begin(); it != entries.end(); it++)
    {
        if (graphic_id != it->graphic_id)
            continue;
        not_queued = 0;
        if (it->is_loaded)
            is_loaded = 1;
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
            i->was_used = 1;
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
