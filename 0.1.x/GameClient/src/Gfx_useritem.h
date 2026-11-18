#ifndef Gfx_useritemH
#define Gfx_useritemH

struct SubItemResource;

// One item picture for the game windows: its surface resource, size, load state and the ticks it has gone undrawn.
struct SubItemWrapper
{
    SubItemResource *resource;
    unsigned short graphic_id;
    short width;
    short height;
    char is_loaded;
    char was_used;
    unsigned short idle_ticks;

    SubItemWrapper(int requested_id);
    ~SubItemWrapper();
    void CreateResource();
    void FreeResource();
};

#endif
