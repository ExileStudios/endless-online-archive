#include <vcl.h>
#pragma hdrstop

#include "Gfx_mapwalls.h"
#include "Tmainform.h"
#include "Gfx_bank.h"
#include "Gfx_wall.h"

#pragma package(smart_init)

WallGraphics::WallGraphics(char preload)
{
    last_index = -1;
    pending_count = 0;
    queue_drained = false;
    library_open = false;
    keep_library_open = preload;
    if (keep_library_open)
        library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_MapWalls).c_str());
}

WallGraphics::~WallGraphics()
{
    if (library_open || keep_library_open)
        FreeLibrary(library);
}

// On a map change: frees the wall library while loads are pending and zeroes the pending count.
// A kept library is opened again.
void WallGraphics::ReopenLibrary()
{
    if (pending_count > 0 || keep_library_open)
    {
        FreeLibrary(library);
        if (!keep_library_open)
            pending_count = 0;
    }
    if (keep_library_open)
        library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_MapWalls).c_str());
}

// Counts, per loaded wall, the frames in a row in which GetGraphic did not fetch it.
void WallGraphics::AgeGraphics()
{
    for (vector<WallWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        if (!it->fetched && it->is_loaded)
            it->unfetched_frames++;
        else
            it->unfetched_frames = 0;
        it->fetched = false;
    }
}

// Loads one waiting wall graphic from its library each call; one that failed to load twice is freed and waits no
// longer.
void WallGraphics::LoadPendingGraphic()
{
    vector<WallWrapper>::iterator it;
    // The call after the wall queue empties closes the library again, unless it is kept open.
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
            library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_MapWalls).c_str());
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

// Frees every wall that has gone unfetched for the purge age.
void WallGraphics::PurgeUnusedGraphics()
{
    bool result;
    // An erase ends the walk, so it starts over after each wall it frees.
    do
    {
        result = false;
        for (vector<WallWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
        {
            if (it->unfetched_frames >= 80)
            {
                it->FreeResource();
                entries.erase(it);
                result = true;
                break;
            }
        }
    } while (result);
}

// Frees every wall and empties the list.
void WallGraphics::ReleaseGraphics()
{
    for (vector<WallWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
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

// Adds a wall that is not listed yet and loads it at once, without the queue.
void WallGraphics::LoadGraphicNow(int graphic_id)
{
    char result = 0;
    vector<WallWrapper>::iterator it;
    for (it = entries.begin(); it != entries.end(); it++)
    {
        if (graphic_id == it->graphic_id)
            result = true;
    }
    if (!result && (Application->Active || !MainForm->config->GetFullscreen()))
    {
        WallWrapper handle(graphic_id);
        entries.insert(entries.end(), handle);
        it = entries.begin();
        while (it != entries.end())
        {
            if (!it->is_loaded && graphic_id == it->graphic_id)
            {
                int module = (int)library;
                it->CreateResource();
                GraphicSize size = it->resource->LoadSurface(module, it->graphic_id + 100);
                it->width = size.width;
                it->height = size.height;
                if (size.width != -1 && size.height != 1)
                {
                    it->is_loaded = true;
                    it->unfetched_frames = 0;
                }
                break;
            }
            it++;
        }
    }
}

// Whether a wall is loaded; the first request for it puts it in the queue.
bool WallGraphics::RequestGraphic(int graphic_id)
{
    vector<WallWrapper>::iterator it;
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
    // No wall is queued while the game runs fullscreen in the background.
    if (not_queued && (Application->Active || !MainForm->config->GetFullscreen()))
        QueueGraphic(graphic_id);
    return is_loaded;
}

// The loaded wall, marked as drawn so AgeGraphics does not count it.
WallWrapper WallGraphics::GetGraphic(int graphic_id)
{
    vector<WallWrapper>::iterator i;
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

GraphicSize WallGraphics::GetGraphicSize(int graphic_id)
{
    GraphicSize size;
    for (vector<WallWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
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

void WallGraphics::QueueGraphic(int graphic_id)
{
    WallWrapper handle(graphic_id);
    entries.insert(entries.end(), handle);
    pending_count++;
}

unsigned int WallGraphics::GetGraphicCount()
{
    return entries.size();
}
