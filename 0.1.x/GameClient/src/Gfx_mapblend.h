#ifndef Gfx_mapblendH
#define Gfx_mapblendH

struct BlendResource;

// One blended overlay: its surface resource, size, load state and the ticks it has gone undrawn.
struct BlendWrapper
{
    BlendResource *resource;
    unsigned short graphic_id;
    short width;
    short height;
    char is_loaded;
    char was_used;
    unsigned short idle_ticks;

    BlendWrapper();
    BlendWrapper(int requested_id);
    ~BlendWrapper();
    void CreateResource();
    void FreeResource();
};

#endif
