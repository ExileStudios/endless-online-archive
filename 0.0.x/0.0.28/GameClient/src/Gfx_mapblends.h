#ifndef Gfx_mapblendsH
#define Gfx_mapblendsH

#include <vector.h>

#include "Gfx_mapblend.h"

// The blended overlays of the map: loaded on demand from their library, one per call, and freed when unused.
struct BlendGraphics
{
    vector<BlendWrapper> entries;
    int last_index;
    int pending_count;
    int queue_drained;
    HMODULE library;
    char library_open;
    char keep_library_open;
    char pad_0x32[6];

    BlendGraphics(char preload);
    ~BlendGraphics();
    void ReopenLibrary();
    void AgeGraphics();
    void LoadPendingGraphic();
    bool PurgeUnusedGraphics();
    void ReleaseGraphics();
    bool RequestGraphic(int graphic_id);
    BlendWrapper GetGraphic(int graphic_id);
    void QueueGraphic(int graphic_id);
    unsigned int GetGraphicCount();
};

#endif
