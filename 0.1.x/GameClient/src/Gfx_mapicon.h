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
    short idle_ticks;

    IconWrapper();
    IconWrapper(int requested_id);
    ~IconWrapper();
    void CreateResource();
};

#endif
