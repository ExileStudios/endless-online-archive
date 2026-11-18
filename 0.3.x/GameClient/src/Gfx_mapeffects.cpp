#include <vcl.h>
#pragma hdrstop

#include "Gfx_mapeffects.h"
#include "Tmainform.h"
#include "Gfx_bank.h"
#include "Gfx_effect.h"

#pragma package(smart_init)

EffectGraphics::EffectGraphics(char preload)
{
    last_index = -1;
    pending_count = 0;
}

EffectGraphics::~EffectGraphics()
{
}

// Counts, per loaded effect graphic, the frames in a row in which GetGraphic did not fetch it.
void EffectGraphics::AgeGraphics()
{
    for (vector<EffectWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        if (!it->fetched && it->is_loaded)
            it->unfetched_frames++;
        else
            it->unfetched_frames = 0;
        it->fetched = false;
    }
}

// Loads one waiting effect graphic from its library each call; one that failed to load twice is freed and waits no
// longer.
void EffectGraphics::LoadPendingGraphic()
{
    vector<EffectWrapper>::iterator it;
    if (pending_count > 0)
    {
        library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_Effects);
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
                    TPoint size = it->resource->LoadSurfaces(open_library, 3 * it->graphic_id + 101);
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
            if (it->unfetched_frames >= 1000)
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
        if (graphic_id == it->graphic_id)
        {
            not_queued = false;
            if (it->is_loaded)
                is_loaded = true;
        }
    }
    // No effect graphic is queued while the game runs fullscreen in the background.
    if (not_queued && (Application->Active || !MainForm->config->GetFullscreen()))
        QueueGraphic(graphic_id);
    return is_loaded;
}

// The loaded effect graphic, marked as drawn so AgeGraphics does not count it.
EffectWrapper *EffectGraphics::GetGraphic(int graphic_id)
{
    vector<EffectWrapper>::iterator i;
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
