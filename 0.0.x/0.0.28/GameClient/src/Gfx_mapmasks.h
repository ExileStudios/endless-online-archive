#ifndef Gfx_mapmasksH
#define Gfx_mapmasksH

#include <vector.h>

#include "Gfx_mapmask.h"
struct GraphicSize;

// The map masks of the map: loaded on demand from their library, one per call, and freed when unused.
struct MaskGraphics
{
    vector<MaskWrapper> entries;
    int last_index;
    int pending_count;
    int queue_drained;
    HMODULE library;
    char library_open;
    char keep_library_open;
    char pad_0x32[6];

    MaskGraphics(char preload);
    ~MaskGraphics();
    void ReopenLibrary();
    void AgeGraphics();
    void LoadPendingGraphic();
    bool PurgeUnusedGraphics();
    void ReleaseGraphics();
    bool RequestGraphic(int graphic_id);
    MaskWrapper GetGraphic(int graphic_id);
    GraphicSize GetGraphicSize(int graphic_id);
    void QueueGraphic(int graphic_id);
    unsigned int GetGraphicCount();
};

#endif
