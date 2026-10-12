#ifndef Gfx_mapeffectsH
#define Gfx_mapeffectsH

#include <vector.h>

#include "Gfx_mapeffect.h"
struct GraphicSize;

// The effect graphics drawn on players: loaded on demand from their library, one per call, and freed when unused.
struct EffectGraphics
{
    vector<EffectWrapper> entries;
    int last_index;
    int pending_count;
    int queue_drained;
    HMODULE library;
    char library_open;
    char keep_library_open;
    char pad_0x32[6];

    EffectGraphics(char preload);
    ~EffectGraphics();
    void AgeGraphics();
    void LoadPendingGraphic();
    void PurgeUnusedGraphics();
    bool RequestGraphic(int graphic_id);
    EffectWrapper GetGraphic(int graphic_id);
    GraphicSize GetGraphicSize(int graphic_id);
    void QueueGraphic(int graphic_id);
};

#endif
