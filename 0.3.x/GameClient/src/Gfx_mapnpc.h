#ifndef Gfx_mapnpcH
#define Gfx_mapnpcH

struct NpcResource;

struct NpcWrapper
{
    NpcResource *resource;
    unsigned short graphic_id;
    char is_loaded;
    char has_resource;
    unsigned char fail_count;
    unsigned char load_stage;
    char was_used;
    unsigned short idle_ticks;

    NpcWrapper();
    NpcWrapper(int requested_id);
    ~NpcWrapper();
    void CreateResource();
    void FreeResource();
};

#endif
