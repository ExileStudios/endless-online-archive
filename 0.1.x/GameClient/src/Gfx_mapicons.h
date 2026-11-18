#ifndef Gfx_mapiconsH
#define Gfx_mapiconsH

#include <new>
#include <vector.h>

#include "Gfx_mapicon.h"

// The spell icons: loaded on demand from their library, one per call, and kept once loaded.
struct IconGraphics
{
    vector<IconWrapper> entries;
    int last_index;
    int pending_count;
    bool queue_drained;
    HMODULE library;
    char library_open;
    char keep_library_open;

    IconGraphics(char preload);
    ~IconGraphics();
    void ReopenLibrary();
    void AgeGraphics();
    void LoadPendingGraphic();
    void PurgeUnusedGraphics();
    bool RequestGraphic(int graphic_id);
    IconWrapper GetGraphic(int graphic_id);
    void QueueGraphic(int graphic_id);
};

#endif
