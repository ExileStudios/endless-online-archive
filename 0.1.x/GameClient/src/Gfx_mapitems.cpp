#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Gfx_bank.h"
#include "Gfx_item.h"
#include "Gfx_subitem.h"
#pragma package(smart_init)

ItemGraphics::ItemGraphics(char preload)
{
    last_index = -1;
    sub_item_last_index = -1;
    pending_count = 0;
    sub_item_pending_count = 0;
    queue_drained = false;
    sub_items_drained = false;
    library_open = false;
    keep_library_open = preload;
    if (keep_library_open)
    {
        library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_MapItems).c_str());
        library_open = true;
    }
}

ItemGraphics::~ItemGraphics()
{
    if (library_open || keep_library_open)
        FreeLibrary(library);
}

// On a map change: frees the item library while map item loads are pending and zeroes their count.
// The library is opened again when it is kept open or item pictures for the windows are still pending.
void ItemGraphics::ReopenLibrary()
{
    if (pending_count > 0 || keep_library_open)
    {
        FreeLibrary(library);
        if (!keep_library_open)
            pending_count = 0;
    }
    if (keep_library_open || sub_item_pending_count > 0)
        library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_MapItems).c_str());
}

// Counts, per loaded map item picture, the frames in a row in which GetGraphic did not fetch it.
void ItemGraphics::AgeGraphics()
{
    for (vector<ItemWrapper>::iterator it = items.begin(); it != items.end(); it++)
    {
        if (!it->was_used && it->is_loaded)
            it->idle_ticks++;
        else
            it->idle_ticks = 0;
        it->was_used = false;
    }
}

// Counts, per loaded window item picture, the frames in a row in which GetSubItemGraphic did not fetch it.
void ItemGraphics::AgeSubItemGraphics()
{
    for (vector<SubItemWrapper>::iterator it = sub_items.begin(); it != sub_items.end(); it++)
    {
        if (!it->was_used && it->is_loaded)
            it->idle_ticks++;
        else
            it->idle_ticks = 0;
        it->was_used = false;
    }
}

// Loads the first queued map item picture and the first queued window item picture, opening the library when no
// load has it open.
void ItemGraphics::LoadPendingGraphics()
{
    // The call after both queues empty closes the library again, unless it is kept open.
    if (queue_drained || sub_items_drained)
    {
        if (library_open && !keep_library_open)
        {
            FreeLibrary(library);
            library_open = false;
        }
        queue_drained = false;
        sub_items_drained = false;
    }
    if (pending_count > 0)
    {
        if (!library_open && !keep_library_open)
        {
            library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_MapItems).c_str());
            library_open = true;
        }
        vector<ItemWrapper>::iterator it;
        for (it = items.begin(); it != items.end(); it++)
        {
            if (!it->is_loaded)
            {
                HMODULE open_library = library;
                it->CreateResource();
                // Each item owns two resource ids in a row: the map picture, then the window picture.
                GraphicSize size = it->resource->LoadSurface((int)open_library, 2 * it->graphic_id + 101);
                it->width = size.width;
                it->height = size.height;
                if (size.width != -1 && size.height != 1)
                {
                    it->is_loaded = true;
                    it->idle_ticks = 0;
                    pending_count--;
                }
                if (pending_count <= 0 && sub_item_pending_count <= 0)
                    queue_drained = true;
                break;
            }
        }
    }
    if (sub_item_pending_count > 0)
    {
        if (!library_open && !keep_library_open)
        {
            library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_MapItems).c_str());
            library_open = true;
        }
        vector<SubItemWrapper>::iterator it;
        for (it = sub_items.begin(); it != sub_items.end(); it++)
        {
            if (!it->is_loaded)
            {
                HMODULE sub_item_library = library;
                it->CreateResource();
                GraphicSize sub_item_size = it->resource->LoadSurface((int)sub_item_library, 2 * it->graphic_id + 102);
                it->width = sub_item_size.width;
                it->height = sub_item_size.height;
                if (sub_item_size.width != -1 && sub_item_size.height != 1)
                {
                    it->is_loaded = true;
                    it->idle_ticks = 0;
                    sub_item_pending_count--;
                }
                if (pending_count <= 0 && sub_item_pending_count <= 0)
                    sub_items_drained = true;
                break;
            }
        }
    }
}

// Frees every item picture that has gone unfetched for its purge age; window pictures are kept a little longer.
bool ItemGraphics::PurgeUnusedGraphics()
{
    char purged;
    bool result;
    // An erase ends the walk, so it starts over after each item picture it frees.
    do
    {
        vector<ItemWrapper>::iterator it;
        purged = false;
        for (it = items.begin(); it != items.end(); it++)
        {
            if (it->idle_ticks >= 40)
            {
                it->FreeResource();
                items.erase(it);
                purged = true;
                break;
            }
        }
    } while (purged);
    do
    {
        vector<SubItemWrapper>::iterator it;
        result = false;
        for (it = sub_items.begin(); it != sub_items.end(); it++)
        {
            if (it->idle_ticks >= 50)
            {
                it->FreeResource();
                sub_items.erase(it);
                result = true;
                break;
            }
        }
    } while (result);
    return result;
}

// Frees every map item picture and empties that list.
void ItemGraphics::ReleaseGraphics()
{
    for (vector<ItemWrapper>::iterator it = items.begin(); it != items.end(); it++)
        it->FreeResource();
    if (GetGraphicCount() >= 1)
    {
        items.clear();
        last_index = -1;
        pending_count = 0;
    }
}

// Frees every window item picture and empties that list.
void ItemGraphics::ReleaseSubItemGraphics()
{
    for (vector<SubItemWrapper>::iterator it = sub_items.begin(); it != sub_items.end(); it++)
        it->FreeResource();
    if (GetSubItemGraphicCount() >= 1)
    {
        sub_items.clear();
        sub_item_last_index = -1;
        sub_item_pending_count = 0;
    }
}

// Whether a map item picture is loaded; the first request for it puts it in the queue.
bool ItemGraphics::RequestGraphic(int graphic_id)
{
    vector<ItemWrapper>::iterator it;
    bool is_loaded = false;
    char not_queued = true;
    for (it = items.begin(); it != items.end(); it++)
    {
        if (graphic_id != it->graphic_id)
            continue;
        not_queued = false;
        if (it->is_loaded)
            is_loaded = true;
    }
    // No map item picture is queued while the game runs fullscreen in the background.
    if (not_queued && (Application->Active || !MainForm->config->GetFullscreen()))
        QueueGraphic(graphic_id);
    return is_loaded;
}

// Whether a window item picture is loaded; the first request for it puts it in the queue.
bool ItemGraphics::RequestSubItemGraphic(int graphic_id)
{
    vector<SubItemWrapper>::iterator it;
    bool is_loaded = false;
    char not_queued = true;
    for (it = sub_items.begin(); it != sub_items.end(); it++)
    {
        if (graphic_id != it->graphic_id)
            continue;
        not_queued = false;
        if (it->is_loaded)
            is_loaded = true;
    }
    if (not_queued && (Application->Active || !MainForm->config->GetFullscreen()))
        QueueSubItemGraphic(graphic_id);
    return is_loaded;
}

// The loaded map item picture, marked as drawn so AgeGraphics does not count it.
ItemWrapper ItemGraphics::GetGraphic(int graphic_id)
{
    vector<ItemWrapper>::iterator i;
    for (i = items.begin(); i != items.end(); i++)
    {
        if (graphic_id == i->graphic_id && i->is_loaded)
        {
            i->was_used = true;
            break;
        }
    }
    return *i;
}

// The loaded window item picture, marked as drawn so AgeSubItemGraphics does not count it.
SubItemWrapper ItemGraphics::GetSubItemGraphic(int graphic_id)
{
    vector<SubItemWrapper>::iterator i;
    for (i = sub_items.begin(); i != sub_items.end(); i++)
    {
        if (graphic_id == i->graphic_id && i->is_loaded)
        {
            i->was_used = true;
            break;
        }
    }
    return *i;
}

void ItemGraphics::QueueGraphic(int graphic_id)
{
    ItemWrapper handle(graphic_id);
    items.insert(items.end(), handle);
    pending_count++;
}

void ItemGraphics::QueueSubItemGraphic(int graphic_id)
{
    SubItemWrapper handle(graphic_id);
    sub_items.insert(sub_items.end(), handle);
    sub_item_pending_count++;
}

unsigned int ItemGraphics::GetGraphicCount()
{
    return items.size();
}

unsigned int ItemGraphics::GetSubItemGraphicCount()
{
    return sub_items.size();
}

// The size of a listed map item picture, as its load reported it.
GraphicSize ItemGraphics::GetGraphicSize(int graphic_id)
{
    GraphicSize size;
    for (vector<ItemWrapper>::iterator it = items.begin(); it != items.end(); it++)
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
