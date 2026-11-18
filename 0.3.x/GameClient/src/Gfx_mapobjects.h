#ifndef Gfx_mapobjectsH
#define Gfx_mapobjectsH

#include <new>
#include <vector.h>

#include "Gfx_mapobject.h"
#include "Gfx_bank.h"

// The map objects of the map: loaded on demand from their library, one per call, and freed when unused.
struct ObjectGraphics
{
    vector<ObjectWrapper> entries;
    int last_index;
    int library;
    int pending_count;

    ObjectGraphics(char preload);
    ~ObjectGraphics();
    void ReopenLibrary();
    void AgeGraphics();
    void LoadPendingGraphic();
    void PurgeUnusedGraphics();
    void ReleaseGraphics();
    bool RequestGraphic(int graphic_id);
    ObjectWrapper * GetGraphic(int graphic_id);
    GraphicSize GetGraphicSize(int graphic_id);
    void QueueGraphic(int graphic_id);
    unsigned int GetGraphicCount();
};

#endif
