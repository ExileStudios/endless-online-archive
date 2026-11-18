#ifndef Gfx_mapiconH
#define Gfx_mapiconH

struct IconResource;

// One spell icon: its surface resource, load state and the ticks it has gone undrawn.
struct IconWrapper
{
    IconResource *resource;
    unsigned short graphic_id;
    char is_loaded;
    char was_used;
    unsigned char idle_ticks;
    char fetched;
    short unfetched_frames;

    IconWrapper();
    IconWrapper(int requested_id);
    ~IconWrapper();
    void CreateResource();
    void FreeResource();
};

#endif
