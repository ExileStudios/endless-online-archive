#ifndef Gfx_mapwallH
#define Gfx_mapwallH

struct WallResource;

// One wall: its surface resource, size, load state and the ticks it has gone undrawn.
struct WallWrapper
{
    WallResource *resource;
    unsigned short graphic_id;
    unsigned short width;
    unsigned short height;
    char is_loaded;
    char was_used;
    unsigned short idle_ticks;

    WallWrapper();
    WallWrapper(int requested_id);
    ~WallWrapper();
    void CreateResource();
    void FreeResource();
};

#endif
