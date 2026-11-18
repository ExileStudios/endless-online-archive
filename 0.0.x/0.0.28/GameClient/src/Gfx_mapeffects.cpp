#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Gfx_bank.h"
#include "Gfx_effect.h"
#pragma package(smart_init)

EffectGraphics::EffectGraphics(char preload)
{
    last_index = -1;
    pending_count = 0;
    queue_drained = false;
    library_open = false;
    keep_library_open = preload;
    if (keep_library_open)
    {
        library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_Effects).c_str());
        library_open = true;
    }
}

EffectGraphics::~EffectGraphics()
{
    if (library_open || keep_library_open)
        FreeLibrary(library);
}

// Counts, per loaded effect graphic, the frames in a row in which GetGraphic did not fetch it.
void EffectGraphics::AgeGraphics()
{
    for (vector<EffectWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        if (!it->was_used && it->is_loaded)
            it->idle_ticks++;
        else
            it->idle_ticks = 0;
        it->was_used = false;
    }
}

void Gfx_mapeffects_UnusedLiterals1()
{
    "";
}

// Loads the first queued effect graphic, opening the library when no load has it open.
void EffectGraphics::LoadPendingGraphic()
{
    // The call after the effect graphic queue empties closes the library again, unless it is kept open.
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
            library = LoadLibraryA(MainForm->strings->GetGfxPath(GfxLibrary_Effects).c_str());
            library_open = true;
        }
        for (vector<EffectWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
        {
            if (!it->is_loaded)
            {
                HMODULE open_library = library;
                it->CreateResource();
                GraphicSize size = it->resource->LoadSurfaces((int)open_library, 3 * it->graphic_id + 101);
                it->width = size.width;
                it->height = size.height;
                if (size.width != -1 && size.height != 1)
                {
                    it->is_loaded = true;
                    it->idle_ticks = 0;
                    pending_count--;
                }
                if (!pending_count)
                    queue_drained = true;
                break;
            }
        }
    }
}

// Frees every effect graphic that has gone unfetched for the purge age.
void EffectGraphics::PurgeUnusedGraphics()
{
    char result;
    // An erase ends the walk, so it starts over after each effect graphic it frees.
    do
    {
        result = 0;
        for (vector<EffectWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
        {
            if (it->idle_ticks >= 120)
            {
                it->FreeResource();
                entries.erase(it);
                result = 1;
                break;
            }
        }
    } while (result);
}

// Whether an effect graphic is loaded; the first request for it puts it in the queue.
bool EffectGraphics::RequestGraphic(int graphic_id)
{
    vector<EffectWrapper>::iterator it;
    bool is_loaded = false;
    char not_queued = true;
    for (it = entries.begin(); it != entries.end(); it++)
    {
        if (graphic_id != it->graphic_id)
            continue;
        not_queued = false;
        if (it->is_loaded)
            is_loaded = true;
    }
    // No effect graphic is queued while the game runs fullscreen in the background.
    if (not_queued && (Application->Active || !MainForm->config->GetFullscreen()))
        QueueGraphic(graphic_id);
    return is_loaded;
}

// The loaded effect graphic, marked as drawn so AgeGraphics does not count it.
EffectWrapper EffectGraphics::GetGraphic(int graphic_id)
{
    vector<EffectWrapper>::iterator i;
    for (i = entries.begin(); i != entries.end(); i++)
    {
        if (graphic_id == i->graphic_id && i->is_loaded)
        {
            i->was_used = true;
            break;
        }
    }
    return *i;
}

GraphicSize EffectGraphics::GetGraphicSize(int graphic_id)
{
    GraphicSize size;
    for (vector<EffectWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
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

void EffectGraphics::QueueGraphic(int graphic_id)
{
    EffectWrapper handle(graphic_id);
    entries.insert(entries.end(), handle);
    pending_count++;
}
