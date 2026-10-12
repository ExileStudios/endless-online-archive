#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Gfx_bank.h"
#include "Gfx_icon.h"
#pragma package(smart_init)

IconGraphics::IconGraphics(char preload)
{
    last_index = -1;
    pending_count = 0;
    queue_drained = 0;
    library_open = 0;
    keep_library_open = preload;
    if (keep_library_open)
    {
        library = LoadLibraryA(MainForm->strings->GetGfxPath(25).c_str());
        library_open = 1;
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
        library = LoadLibraryA(MainForm->strings->GetGfxPath(25).c_str());
}

// Counts, per loaded spell icon, the frames in a row in which GetGraphic did not fetch it.
void IconGraphics::AgeGraphics()
{
    for (vector<IconWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        if (!it->was_used && it->is_loaded)
            it->idle_ticks++;
        else
            it->idle_ticks = 0;
        it->was_used = 0;
    }
}

// Loads the first queued spell icon, opening the library when no load has it open.
void IconGraphics::LoadPendingGraphic()
{
    // The call after the spell icon queue empties closes the library again, unless it is kept open.
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
            library = LoadLibraryA(MainForm->strings->GetGfxPath(25).c_str());
            library_open = 1;
        }
        for (vector<IconWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
        {
            if (!it->is_loaded)
            {
                HMODULE open_library = library;
                it->CreateResource();
                GraphicSize size = it->resource->LoadSurface((int)open_library, it->graphic_id + 100);
                it->is_loaded = 1;
                it->idle_ticks = 0;
                pending_count--;
                if (!pending_count)
                    queue_drained = 1;
                break;
            }
        }
    }
}

void IconGraphics::PurgeUnusedGraphics()
{

}

// Whether a spell icon is loaded; the first request for it puts it in the queue.
bool IconGraphics::RequestGraphic(int graphic_id)
{
    bool is_loaded = 0;
    char not_queued = 1;
    vector<IconWrapper>::iterator it;
    for (it = entries.begin(); it != entries.end(); it++)
    {
        if (graphic_id != it->graphic_id)
            continue;
        not_queued = 0;
        if (it->is_loaded)
            is_loaded = 1;
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
            i->was_used = 1;
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
