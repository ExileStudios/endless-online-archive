#include <vcl.h>
#pragma hdrstop

#include "Gfx_mapnpcs.h"
#include "Tmainform.h"
#include "Gfx_npc.h"

#pragma package(smart_init)

NpcGraphics::NpcGraphics(char preload)
{
    last_index = -1;
    pending_count = 0;
}

NpcGraphics::~NpcGraphics()
{
}

void NpcGraphics::ReopenLibrary()
{
}

// Counts, per loaded npc graphic, the frames in a row in which GetGraphic did not fetch it.
void NpcGraphics::AgeGraphics()
{
    for (vector<NpcWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        if (!it->was_used && it->is_loaded)
            it->idle_ticks++;
        else
            it->idle_ticks = 0;
        it->was_used = false;
    }
}

// Loads the frames of the next npc graphic waiting to load: standing, idle when the npc has it, walking and attack,
// front and back, and their mirror images.
void NpcGraphics::LoadPendingGraphic()
{
    NpcWrapper *it;
    int frame;
    unsigned char pending_seen;
    pending_seen = 0;
    if (pending_count > 0)
    {
        library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_Npcs);
        it = entries.begin();
        for (; it != entries.end(); it++)
        {
            if (!it->is_loaded)
            {
                pending_seen++;
                if (pending_seen > 2)
                    return;
                if (it->fail_count > 1)
                {
                    if (it->fail_count == 2)
                    {
                        it->fail_count = 250;
                        it->FreeResource();
                        pending_count--;
                    }
                }
                else
                {
                    int open_library = library;
                    int resource_id = 40 * it->graphic_id + 61;
                    if (!it->load_stage)
                    {
                        NpcWrapper entry_copy = *it;
                        it->CreateResource();
                        TPoint size = it->resource->CreateSheets(open_library, resource_id);
                        it->load_stage++;
                    }
                    if (it->load_stage == 1)
                    {
                        if (!it->resource->LoadFrontFrame(0, open_library, resource_id))
                        {
                            it->fail_count++;
                            continue;
                        }
                        if (!it->resource->LoadBackFrame(0, open_library, resource_id + 2))
                        {
                            it->fail_count++;
                            continue;
                        }
                        it->resource->MirrorFrame(0);
                        int npc_index = it->graphic_id - 1;
                        if (MainForm->npcs->has_idle_animation[npc_index >= 0 ? npc_index : 0])
                        {
                            if (!it->resource->LoadFrontFrame(1, open_library, resource_id + 1))
                            {
                                it->fail_count++;
                                continue;
                            }
                            if (!it->resource->LoadBackFrame(1, open_library, resource_id + 3))
                            {
                                it->fail_count++;
                                continue;
                            }
                            it->resource->MirrorFrame(1);
                        }
                        it->load_stage++;
                        break;
                    }
                    if (it->load_stage == 2)
                    {
                        for (frame = 0; frame < 4; frame++)
                        {
                            if (!it->resource->LoadFrontFrame(frame + 2, open_library, resource_id + frame + 4))
                                it->fail_count++;
                            else if (!it->resource->LoadBackFrame(frame + 2, open_library, resource_id + frame + 8))
                                it->fail_count++;
                            else
                                it->resource->MirrorFrame(frame + 2);
                        }
                        it->load_stage++;
                        break;
                    }
                    if (it->load_stage == 3)
                    {
                        if (!it->resource->LoadFrontFrame(6, open_library, resource_id + 12))
                        {
                            it->fail_count++;
                            continue;
                        }
                        if (!it->resource->LoadBackFrame(6, open_library, resource_id + 14))
                        {
                            it->fail_count++;
                            continue;
                        }
                        if (!it->resource->LoadFrontFrame(7, open_library, resource_id + 13))
                        {
                            it->fail_count++;
                            continue;
                        }
                        if (!it->resource->LoadBackFrame(7, open_library, resource_id + 15))
                        {
                            it->fail_count++;
                            continue;
                        }
                        if (!it->resource->LoadFrontFrame(8, open_library, resource_id + 16))
                        {
                            it->fail_count++;
                            continue;
                        }
                        if (!it->resource->LoadBackFrame(8, open_library, resource_id + 17))
                        {
                            it->fail_count++;
                            continue;
                        }
                        it->resource->MirrorFrame(6);
                        it->resource->MirrorFrame(7);
                        it->resource->MirrorFrame(8);
                        it->load_stage++;
                    }
                    if (it->load_stage == 4)
                    {
                        it->is_loaded = true;
                        it->idle_ticks = 0;
                        it->fail_count = 0;
                        it->load_stage++;
                        pending_count--;
                        break;
                    }
                }
            }
        }
    }
}

// Frees and removes every npc graphic unused for 1000 ticks or more.
void NpcGraphics::PurgeUnusedGraphics()
{
    char erased;
    do
    {
        erased = false;
        for (vector<NpcWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
        {
            if (it->idle_ticks >= 1000)
            {
                it->FreeResource();
                entries.erase(it);
                erased = true;
                break;
            }
        }
    } while (erased);
}

// Frees every npc graphic and empties the list.
void NpcGraphics::ReleaseGraphics()
{
    for (vector<NpcWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
        it->FreeResource();
    if (GetGraphicCount() >= 1)
    {
        entries.clear();
        last_index = -1;
        pending_count = 0;
    }
}

// True when the npc graphic is loaded; one not listed yet is queued while the game window is active or windowed.
bool NpcGraphics::RequestGraphic(int graphic_id)
{
    vector<NpcWrapper>::iterator it;
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
    if (not_queued && (Application->Active || !MainForm->config->GetFullscreen()))
        QueueGraphic(graphic_id);
    return is_loaded;
}

// The loaded npc graphic with this id, marked as used, or null.
NpcWrapper *NpcGraphics::GetGraphic(int graphic_id)
{
    for (vector<NpcWrapper>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        if (graphic_id == it->graphic_id && it->is_loaded)
        {
            it->was_used = true;
            return it;
        }
    }
    return 0;
}

void NpcGraphics::QueueGraphic(int graphic_id)
{
    NpcWrapper handle(graphic_id);
    entries.insert(entries.end(), handle);
    pending_count++;
}

unsigned int NpcGraphics::GetGraphicCount()
{
    return entries.size();
}
