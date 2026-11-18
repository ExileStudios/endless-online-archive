#ifndef Gfx_mapnpcsH
#define Gfx_mapnpcsH

#include <new>
#include <vector.h>

#include "Gfx_mapnpc.h"

// Draws the npcs in the map view: the frame of each npc's action, its health bar, damage number and balloon text.
struct NpcGraphics
{
    vector<NpcWrapper> entries;
    int last_index;
    int library;
    int pending_count;

    NpcGraphics(char preload);
    ~NpcGraphics();
    void ReopenLibrary();
    void AgeGraphics();
    void LoadPendingGraphic();
    void PurgeUnusedGraphics();
    void ReleaseGraphics();
    bool RequestGraphic(int graphic_id);
    NpcWrapper * GetGraphic(int graphic_id);
    void QueueGraphic(int graphic_id);
    unsigned int GetGraphicCount();
};

#endif
