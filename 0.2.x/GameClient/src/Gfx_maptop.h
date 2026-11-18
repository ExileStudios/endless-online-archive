#ifndef Gfx_maptopH
#define Gfx_maptopH

struct TopResource;

// One top layer graphic: its surface resource, size, load state and the ticks it has gone undrawn.
struct TopWrapper
{
    TopResource *resource;
    unsigned short graphic_id;
    char is_loaded;
    char was_used;
    unsigned char idle_ticks;
    char fetched;
    unsigned short unfetched_frames;

    TopWrapper();
    TopWrapper(int requested_id);
    ~TopWrapper();
    void CreateResource();
    void FreeResource();
};

#endif
