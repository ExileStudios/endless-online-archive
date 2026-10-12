#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Gfx_bank.h"
#include "Gfx_object.h"
#pragma package(smart_init)

ObjectGraphics::ObjectGraphics(char preload)
{
    last_index = -1;
    pending_count = 0;
    queue_drained = 0;
    library_open = 0;
    keep_library_open = preload;
    if (keep_library_open)
    {
        library = LoadLibraryA(MainForm->strings->GetGfxPath(4).c_str());
        library_open = 1;
    }
}

ObjectGraphics::~ObjectGraphics()
{
    if (library_open || keep_library_open)
        FreeLibrary(library);
}

// On a map change: frees the map object library while loads are pending and zeroes the pending count.
// A kept library is opened again.
void ObjectGraphics::ReopenLibrary()
{
    if (pending_count > 0 || keep_library_open)
    {
        FreeLibrary(library);
        if (!keep_library_open)
            pending_count = 0;
    }
    if (keep_library_open)
        library = LoadLibraryA(MainForm->strings->GetGfxPath(4).c_str());
}

// Counts, per loaded map object, the frames in a row in which GetGraphic did not fetch it.
void ObjectGraphics::AgeGraphics()
{
    for (vector<ObjectWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        if (!it->was_used && it->is_loaded)
            it->idle_ticks++;
        else
            it->idle_ticks = 0;
        it->was_used = 0;
    }
}

// Loads the first queued map object, opening the library when no load has it open.
void ObjectGraphics::LoadPendingGraphic()
{
    // The call after the map object queue empties closes the library again, unless it is kept open.
    if (queue_drained)
    {
        if (library_open && !keep_library_open)
        {
            FreeLibrary(library);
            library_open = 0;
        }
        queue_drained = 0;
        MainForm->redraw_needed = true;
    }
    if (pending_count > 0)
    {
        if (!library_open && !keep_library_open)
        {
            library = LoadLibraryA(MainForm->strings->GetGfxPath(4).c_str());
            library_open = 1;
        }
        for (vector<ObjectWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
        {
            if (!it->is_loaded)
            {
                HMODULE open_library = library;
                it->CreateResource();
                GraphicSize size = it->resource->LoadSurface((int)open_library, it->graphic_id + 100);
                it->width = size.width;
                it->height = size.height;
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

// Frees every map object that has gone unfetched for the purge age.
bool ObjectGraphics::PurgeUnusedGraphics()
{
    bool result;
    // An erase ends the walk, so it starts over after each map object it frees.
    do
    {
        result = 0;
        for (vector<ObjectWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
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

// Frees every map object and empties the list.
void ObjectGraphics::ReleaseGraphics()
{
    for (vector<ObjectWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
        it->FreeResource();
    if (GetGraphicCount() >= 1)
    {
        entries.clear();
        last_index = -1;
        pending_count = 0;
    }
}

// Whether a map object is loaded; the first request for it puts it in the queue.
bool ObjectGraphics::RequestGraphic(int graphic_id)
{
    bool is_loaded = 0;
    char not_queued = 1;
    vector<ObjectWrapper>::iterator it;
    for (it = entries.begin(); it != entries.end(); it++)
    {
        if (graphic_id != it->graphic_id)
            continue;
        not_queued = 0;
        if (it->is_loaded)
            is_loaded = 1;
    }
    // No map object is queued while the game runs fullscreen in the background.
    if (not_queued && (Application->Active || !MainForm->config->GetFullscreen()))
        QueueGraphic(graphic_id);
    return is_loaded;
}

// The loaded map object, marked as drawn so AgeGraphics does not count it.
ObjectWrapper ObjectGraphics::GetGraphic(int graphic_id)
{
    vector<ObjectWrapper>::iterator i;
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

GraphicSize ObjectGraphics::GetGraphicSize(int graphic_id)
{
    GraphicSize size;
    for (vector<ObjectWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
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

void ObjectGraphics::QueueGraphic(int graphic_id)
{
    ObjectWrapper handle(graphic_id);
    entries.insert(entries.end(), handle);
    pending_count++;
}

unsigned int ObjectGraphics::GetGraphicCount()
{
    return entries.size();
}
