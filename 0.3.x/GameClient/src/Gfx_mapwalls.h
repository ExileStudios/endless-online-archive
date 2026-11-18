#ifndef Gfx_mapwallsH
#define Gfx_mapwallsH

#include <new>
#include <vector.h>

#include "Gfx_mapwall.h"
#include "Gfx_bank.h"

// The walls of the map: loaded on demand from their library, one per call, and freed when unused.
struct WallGraphics
{
    vector<WallWrapper> entries;
    int last_index;
    int library;
    int pending_count;

    WallGraphics(char preload);
    ~WallGraphics();
    void ReopenLibrary();
    void AgeGraphics();
    void LoadPendingGraphic();
    void PurgeUnusedGraphics();
    void ReleaseGraphics();
    void LoadGraphicNow(int graphic_id);
    bool RequestGraphic(int graphic_id);
    WallWrapper * GetGraphic(int graphic_id);
    GraphicSize GetGraphicSize(int graphic_id);
    void QueueGraphic(int graphic_id);
    unsigned int GetGraphicCount();
};

#endif
