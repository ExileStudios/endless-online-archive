#include <vcl.h>
#pragma hdrstop

#include "Gfx_mapmasks.h"
#include "Tmainform.h"
#include "Gfx_bank.h"
#include "Gfx_mask.h"

#pragma package(smart_init)

MaskGraphics::MaskGraphics(char preload)
{
    last_index = -1;
    pending_count = 0;
    queue_drained = false;
    library_open = false;
    keep_library_open = preload;
    if (keep_library_open)
    {
        library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_MapMasks).c_str());
        library_open = false;
    }
}

MaskGraphics::~MaskGraphics()
{
    if (library_open || keep_library_open)
        FreeLibrary(library);
}

// On a map change: frees the map mask library while loads are pending and zeroes the pending count.
// A kept library is opened again.
void MaskGraphics::ReopenLibrary()
{
    if (pending_count > 0 || keep_library_open)
    {
        FreeLibrary(library);
        if (!keep_library_open)
            pending_count = 0;
    }
    if (keep_library_open)
        library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_MapMasks).c_str());
}

// Counts, per loaded map mask, the frames in a row in which GetGraphic did not fetch it.
void MaskGraphics::AgeGraphics()
{
    for (vector<MaskWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        if (!it->fetched && it->is_loaded)
            it->unfetched_frames++;
        else
            it->unfetched_frames = 0;
        it->fetched = false;
    }
}

// Loads one waiting mask graphic from its library each call; one that failed to load twice is freed and waits no
// longer.
void MaskGraphics::LoadPendingGraphic()
{
    vector<MaskWrapper>::iterator it;
    // The call after the map mask queue empties closes the library again, unless it is kept open.
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
            library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_MapMasks).c_str());
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
                    int library_handle = (int)library;
                    it->CreateResource();
                    GraphicSize size = it->resource->LoadSurface(library_handle, it->graphic_id + 100);
                    it->width = size.width;
                    it->height = size.height;
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

// Frees every map mask that has gone unfetched for the purge age.
void MaskGraphics::PurgeUnusedGraphics()
{
    bool result;
    // An erase ends the walk, so it starts over after each map mask it frees.
    do
    {
        result = false;
        for (vector<MaskWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
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

// Frees every map mask and empties the list.
void MaskGraphics::ReleaseGraphics()
{
    for (vector<MaskWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
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

// Whether a map mask is loaded; the first request for it puts it in the queue.
bool MaskGraphics::RequestGraphic(int graphic_id)
{
    vector<MaskWrapper>::iterator it;
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
    // No map mask is queued while the game runs fullscreen in the background.
    if (not_queued && (Application->Active || !MainForm->config->GetFullscreen()))
        QueueGraphic(graphic_id);
    return is_loaded;
}

// The loaded map mask, marked as drawn so AgeGraphics does not count it.
MaskWrapper MaskGraphics::GetGraphic(int graphic_id)
{
    vector<MaskWrapper>::iterator i;
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

GraphicSize MaskGraphics::GetGraphicSize(int graphic_id)
{
    GraphicSize size;
    for (vector<MaskWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        if (graphic_id == it->graphic_id)
        {
            size.width = it->width;
            size.height = it->height;
            break;
        }
    }
    return size;
}

void MaskGraphics::QueueGraphic(int graphic_id)
{
    MaskWrapper handle(graphic_id);
    entries.insert(entries.end(), handle);
    pending_count++;
}

unsigned int MaskGraphics::GetGraphicCount()
{
    return entries.size();
}
