#ifndef Gfx_mapmasksH
#define Gfx_mapmasksH

#include <new>
#include <vector.h>

#include "Gfx_mapmask.h"
#include "Gfx_bank.h"

// The map masks of the map: loaded on demand from their library, one per call, and freed when unused.
struct MaskGraphics
{
    vector<MaskWrapper> entries;
    int last_index;
    int library;
    int pending_count;

    MaskGraphics(char preload);
    ~MaskGraphics();
    void ReopenLibrary();
    void AgeGraphics();
    void LoadPendingGraphic();
    void PurgeUnusedGraphics();
    void ReleaseGraphics();
    bool RequestGraphic(int graphic_id);
    MaskWrapper * GetGraphic(int graphic_id);
    GraphicSize GetGraphicSize(int graphic_id);
    void QueueGraphic(int graphic_id);
    unsigned int GetGraphicCount();
};

#endif
