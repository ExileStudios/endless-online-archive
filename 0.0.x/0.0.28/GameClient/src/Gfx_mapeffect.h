#ifndef Gfx_mapeffectH
#define Gfx_mapeffectH

struct EffectResource;

// One effect graphic: its surface resource, size, load state and the ticks it has gone undrawn.
struct EffectWrapper
{
    EffectResource *resource;
    unsigned short graphic_id;
    unsigned short width;
    unsigned short height;
    char is_loaded;
    char was_used;
    unsigned short idle_ticks;

    EffectWrapper();
    EffectWrapper(int requested_id);
    ~EffectWrapper();
    void CreateResource();
    void FreeResource();
};

#endif
