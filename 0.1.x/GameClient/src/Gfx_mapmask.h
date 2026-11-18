#ifndef Gfx_mapmaskH
#define Gfx_mapmaskH

struct MaskResource;

// One map mask: its surface resource, size, load state and the ticks it has gone undrawn.
struct MaskWrapper
{
    MaskResource *resource;
    unsigned short graphic_id;
    unsigned short width;
    unsigned short height;
    char is_loaded;
    char was_used;
    unsigned char idle_ticks;
    char fetched;
    unsigned short unfetched_frames;

    MaskWrapper();
    MaskWrapper(int requested_id);
    ~MaskWrapper();
    void CreateResource();
    void FreeResource();
};

#endif
