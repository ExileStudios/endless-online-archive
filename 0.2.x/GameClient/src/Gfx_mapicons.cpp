#include <vcl.h>
#pragma hdrstop

#include "Gfx_mapicons.h"
#include "Tmainform.h"
#include "Gfx_bank.h"
#include "Gfx_icon.h"

#pragma package(smart_init)

IconGraphics::IconGraphics(char preload)
{
    last_index = -1;
    pending_count = 0;
    queue_drained = false;
    library_open = false;
    keep_library_open = preload;
    if (keep_library_open)
    {
        library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_MapIcons).c_str());
        library_open = true;
    }
}

IconGraphics::~IconGraphics()
{
    if (library_open || keep_library_open)
        FreeLibrary(library);
}

// On logout: frees the spell icon library while loads are pending and zeroes the pending count.
// A kept library is opened again.
void IconGraphics::ReopenLibrary()
{
    if (pending_count > 0 || keep_library_open)
    {
        FreeLibrary(library);
        if (!keep_library_open)
            pending_count = 0;
    }
    if (keep_library_open)
        library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_MapIcons).c_str());
}

// Counts, per loaded spell icon, the frames in a row in which GetGraphic did not fetch it.
void IconGraphics::AgeGraphics()
{
    for (vector<IconWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        if (!it->fetched && it->is_loaded)
            it->unfetched_frames++;
        else
            it->unfetched_frames = 0;
        it->fetched = false;
    }
}

// Loads one waiting icon graphic from its library each call; one that failed to load twice is freed and waits no
// longer.
void IconGraphics::LoadPendingGraphic()
{
    vector<IconWrapper>::iterator it;
    // The call after the spell icon queue empties closes the library again, unless it is kept open.
    if (queue_drained)
    {
        if (library_open && !keep_library_open)
        {
            FreeLibrary(library);
            library_open = false;
        }
        queue_drained = false;
        MainForm->redraw_needed = true;
    }
    if (pending_count > 0)
    {
        if (!library_open && !keep_library_open)
        {
            library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_MapIcons).c_str());
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

void IconGraphics::PurgeUnusedGraphics()
{
}

// Whether a spell icon is loaded; the first request for it puts it in the queue.
bool IconGraphics::RequestGraphic(int graphic_id)
{
    vector<IconWrapper>::iterator it;
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
    // No spell icon is queued while the game runs fullscreen in the background.
    if (not_queued && (Application->Active || !MainForm->config->GetFullscreen()))
        QueueGraphic(graphic_id);
    return is_loaded;
}

// The loaded spell icon, marked as drawn so AgeGraphics does not count it.
IconWrapper IconGraphics::GetGraphic(int graphic_id)
{
    vector<IconWrapper>::iterator i;
    for (i = entries.begin(); i != entries.end(); i++)
    {
        if (graphic_id == i->graphic_id && i->is_loaded)
        {
            i->unfetched_frames = 0;
            i->fetched = true;
            break;
        }
    }
    return *i;
}

void IconGraphics::QueueGraphic(int graphic_id)
{
    IconWrapper handle(graphic_id);
    entries.insert(entries.end(), handle);
    pending_count++;
}
