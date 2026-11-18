#ifndef Gfx_maptilesH
#define Gfx_maptilesH

#include <new>
#include <vector.h>

#include "Gfx_maptile.h"

// The ground tiles of the map: loaded on demand from their library, one per call, and freed when unused.
struct TileGraphics
{
    vector<TileWrapper> entries;
    int last_index;
    int library;
    int pending_count;

    TileGraphics(char preload);
    ~TileGraphics();
    void ReopenLibrary();
    void AgeGraphics();
    void LoadPendingGraphic();
    void PurgeUnusedGraphics();
    void ReleaseGraphics();
    bool RequestGraphic(int graphic_id);
    TileWrapper * GetGraphic(int graphic_id);
    void QueueGraphic(int graphic_id);
    unsigned int GetGraphicCount();
};

#endif
