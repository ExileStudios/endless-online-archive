#ifndef Gfx_maptilesH
#define Gfx_maptilesH

#include <vector.h>

#include "Gfx_maptile.h"

// The ground tiles of the map: loaded on demand from their library, one per call, and freed when unused.
struct TileGraphics
{
    vector<TileWrapper> entries;
    int last_index;
    int pending_count;
    int queue_drained;
    HMODULE library;
    char library_open;
    char keep_library_open;
    char pad_0x32[6];

    TileGraphics(char preload);
    ~TileGraphics();
    void ReopenLibrary();
    void AgeGraphics();
    void LoadPendingGraphic();
    bool PurgeUnusedGraphics();
    void ReleaseGraphics();
    bool RequestGraphic(int graphic_id);
    TileWrapper GetGraphic(int graphic_id);
    void QueueGraphic(int graphic_id);
    unsigned int GetGraphicCount();
};

#endif
