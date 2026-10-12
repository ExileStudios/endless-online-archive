#ifndef Gfx_maptileH
#define Gfx_maptileH

struct TileResource;

// One ground tile: its surface resource, size, load state and the ticks it has gone undrawn.
struct TileWrapper
{
    TileResource *resource;
    unsigned short graphic_id;
    char is_loaded;
    char was_used;
    unsigned short idle_ticks;

    TileWrapper(int requested_id);
    ~TileWrapper();
    void CreateResource();
    void FreeResource();
};

#endif
