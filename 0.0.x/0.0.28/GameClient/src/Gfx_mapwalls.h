#ifndef Gfx_mapwallsH
#define Gfx_mapwallsH

#include <vector.h>

#include "Gfx_mapwall.h"
struct GraphicSize;

// The walls of the map: loaded on demand from their library, one per call, and freed when unused.
struct WallGraphics
{
    vector<WallWrapper> entries;
    int last_index;
    int pending_count;
    int queue_drained;
    HMODULE library;
    char library_open;
    char keep_library_open;
    char pad_0x32[6];

    WallGraphics(char preload);
    ~WallGraphics();
    void ReopenLibrary();
    void AgeGraphics();
    void LoadPendingGraphic();
    bool PurgeUnusedGraphics();
    void ReleaseGraphics();
    void LoadGraphicNow(int graphic_id);
    bool RequestGraphic(int graphic_id);
    WallWrapper GetGraphic(int graphic_id);
    GraphicSize GetGraphicSize(int graphic_id);
    void QueueGraphic(int graphic_id);
    unsigned int GetGraphicCount();
};

#endif
