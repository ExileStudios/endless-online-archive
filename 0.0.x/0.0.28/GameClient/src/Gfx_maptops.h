#ifndef Gfx_maptopsH
#define Gfx_maptopsH

#include <vector.h>

#include "Gfx_maptop.h"

// The top layer graphics of the map: loaded on demand from their library, one per call, and freed when unused.
struct TopGraphics
{
    vector<TopWrapper> entries;
    int last_index;
    int pending_count;
    int queue_drained;
    HMODULE library;
    char library_open;
    char keep_library_open;
    char pad_0x32[6];

    TopGraphics(char preload);
    ~TopGraphics();
    void ReopenLibrary();
    void AgeGraphics();
    void LoadPendingGraphic();
    bool PurgeUnusedGraphics();
    void ReleaseGraphics();
    bool RequestGraphic(int graphic_id);
    TopWrapper GetGraphic(int graphic_id);
    void QueueGraphic(int graphic_id);
    unsigned int GetGraphicCount();
};

#endif
