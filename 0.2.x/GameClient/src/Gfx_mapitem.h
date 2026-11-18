#ifndef Gfx_mapitemH
#define Gfx_mapitemH

struct ItemResource;

// One item picture for the map: its surface resource, size, load state and the ticks it has gone undrawn.
struct ItemWrapper
{
    ItemResource *resource;
    unsigned short graphic_id;
    unsigned short width;
    unsigned short height;
    char is_loaded;
    char was_used;
    unsigned char idle_ticks;
    char fetched;
    unsigned short unfetched_frames;

    ItemWrapper();
    ItemWrapper(int requested_id);
    ~ItemWrapper();
    void CreateResource();
    void FreeResource();
};

#endif
