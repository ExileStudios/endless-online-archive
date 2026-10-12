#ifndef Gfx_mapobjectsH
#define Gfx_mapobjectsH

#include <vector.h>

#include "Gfx_mapobject.h"
struct GraphicSize;

// The map objects of the map: loaded on demand from their library, one per call, and freed when unused.
struct ObjectGraphics
{
    vector<ObjectWrapper> entries;
    int last_index;
    int pending_count;
    int queue_drained;
    HMODULE library;
    char library_open;
    char keep_library_open;
    char pad_0x32[6];

    ObjectGraphics(char preload);
    ~ObjectGraphics();
    void ReopenLibrary();
    void AgeGraphics();
    void LoadPendingGraphic();
    bool PurgeUnusedGraphics();
    void ReleaseGraphics();
    bool RequestGraphic(int graphic_id);
    ObjectWrapper GetGraphic(int graphic_id);
    GraphicSize GetGraphicSize(int graphic_id);
    void QueueGraphic(int graphic_id);
    unsigned int GetGraphicCount();
};

#endif
