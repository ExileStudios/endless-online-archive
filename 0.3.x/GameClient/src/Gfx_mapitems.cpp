#include <vcl.h>
#pragma hdrstop

#include "Gfx_mapitems.h"
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
}

ItemGraphics::~ItemGraphics()
{
}

// On a map change: frees the item library while map item loads are pending and zeroes their count.
// The library is opened again when it is kept open or item pictures for the windows are still pending.
void ItemGraphics::ReopenLibrary()
{
}

// Counts, per loaded map item picture, the frames in a row in which GetGraphic did not fetch it.
void ItemGraphics::AgeGraphics()
{
    for (vector<ItemWrapper>::iterator it = items.begin(); it != items.end(); it++)
    {
        if (!it->fetched && it->is_loaded)
            it->unfetched_frames++;
        else
            it->unfetched_frames = 0;
        it->fetched = false;
    }
}

// Counts, per loaded window item picture, the frames in a row in which GetSubItemGraphic did not fetch it.
void ItemGraphics::AgeSubItemGraphics()
{
    for (vector<SubItemWrapper>::iterator it = sub_items.begin(); it != sub_items.end(); it++)
    {
        if (!it->fetched && it->is_loaded)
            it->unfetched_frames++;
        else
            it->unfetched_frames = 0;
        it->fetched = false;
    }
}

// Loads the item and inventory pictures waiting to load, each into its own surface, from the item library.
void ItemGraphics::LoadPendingGraphics()
{
    vector<ItemWrapper>::iterator it;
    vector<SubItemWrapper>::iterator sub_item;
    if (pending_count > 0)
    {
        library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MapItems);
        it = items.begin();
        while (it != items.end())
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
                    // Each item owns two resource ids in a row: the map picture, then the window picture.
                    TPoint size = it->resource->LoadSurface(open_library, 2 * it->graphic_id + 101);
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
    if (sub_item_pending_count > 0)
    {
        library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MapItems);
        sub_item = sub_items.begin();
        while (sub_item != sub_items.end())
        {
            if (!sub_item->is_loaded)
            {
                if (sub_item->idle_ticks > 1)
                {
                    if (sub_item->idle_ticks == 2)
                    {
                        sub_item->idle_ticks = 250;
                        sub_item->FreeResource();
                        sub_item_pending_count--;
                    }
                }
                else
                {
                    int sub_open_library = library;
                    sub_item->CreateResource();
                    TPoint sub_item_size =
                        sub_item->resource->LoadSurface(sub_open_library, 2 * sub_item->graphic_id + 102);
                    sub_item->width = sub_item_size.x;
                    sub_item->height = sub_item_size.y;
                    if (sub_item_size.x == -1)
                        sub_item->idle_ticks++;
                    else
                    {
                        sub_item->is_loaded = true;
                        sub_item->unfetched_frames = 0;
                        sub_item->idle_ticks = 0;
                        sub_item_pending_count--;
                    }
                    break;
                }
            }
            sub_item++;
        }
    }
}

// Frees every item picture that has gone unfetched for its purge age; window pictures are kept a little longer.
void ItemGraphics::PurgeUnusedGraphics()
{
    char purged;
    bool result;
    // An erase ends the walk, so it starts over after each item picture it frees.
    do
    {
        purged = false;
        vector<ItemWrapper>::iterator it;
        for (it = items.begin(); it != items.end(); it++)
        {
            if (it->unfetched_frames >= 2000)
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
        result = false;
        for (vector<SubItemWrapper>::iterator sub_item = sub_items.begin(); sub_item != sub_items.end(); sub_item++)
        {
            if (sub_item->unfetched_frames >= 3000)
            {
                sub_item->FreeResource();
                sub_items.erase(sub_item);
                result = true;
                break;
            }
        }
    } while (result);
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
        if (graphic_id == it->graphic_id)
        {
            not_queued = false;
            if (it->is_loaded)
                is_loaded = true;
        }
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
        if (graphic_id == it->graphic_id)
        {
            not_queued = false;
            if (it->is_loaded)
                is_loaded = true;
        }
    }
    if (not_queued && (Application->Active || !MainForm->config->GetFullscreen()))
        QueueSubItemGraphic(graphic_id);
    return is_loaded;
}

// The loaded map item picture, marked as drawn so AgeGraphics does not count it.
ItemWrapper *ItemGraphics::GetGraphic(int graphic_id)
{
    vector<ItemWrapper>::iterator i;
    for (i = items.begin(); i != items.end(); i++)
    {
        if (graphic_id == i->graphic_id && i->is_loaded)
        {
            i->fetched = true;
            return i;
        }
    }
    return 0;
}

// The loaded window item picture, marked as drawn so AgeSubItemGraphics does not count it.
SubItemWrapper *ItemGraphics::GetSubItemGraphic(int graphic_id)
{
    vector<SubItemWrapper>::iterator i;
    for (i = sub_items.begin(); i != sub_items.end(); i++)
    {
        if (graphic_id == i->graphic_id && i->is_loaded)
        {
            i->fetched = true;
            return i;
        }
    }
    return 0;
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
