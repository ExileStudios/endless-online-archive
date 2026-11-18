#ifndef Gfx_maptopsH
#define Gfx_maptopsH

#include <new>
#include <vector.h>

#include "Gfx_maptop.h"

// The top layer graphics of the map: loaded on demand from their library, one per call, and freed when unused.
struct TopGraphics
{
    vector<TopWrapper> entries;
    int last_index;
    int library;
    int pending_count;

    TopGraphics(char preload);
    ~TopGraphics();
    void ReopenLibrary();
    void AgeGraphics();
    void LoadPendingGraphic();
    void PurgeUnusedGraphics();
    void ReleaseGraphics();
    bool RequestGraphic(int graphic_id);
    TopWrapper * GetGraphic(int graphic_id);
    void QueueGraphic(int graphic_id);
    unsigned int GetGraphicCount();
};

#endif
