#ifndef Gfx_mapobjectH
#define Gfx_mapobjectH

struct ObjectResource;

// One map object: its surface resource, size, load state and the ticks it has gone undrawn.
struct ObjectWrapper
{
    ObjectResource *resource;
    unsigned short graphic_id;
    unsigned short width;
    unsigned short height;
    char is_loaded;
    char was_used;
    unsigned short idle_ticks;

    ObjectWrapper(int requested_id);
    ~ObjectWrapper();
    void CreateResource();
    void FreeResource();
};

#endif
