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
    unsigned char idle_ticks;
    char fetched;
    unsigned short unfetched_frames;

    ObjectWrapper(int requested_id);
    ~ObjectWrapper();
    void CreateResource();
    void FreeResource();
};


// Shared body: a class defines its FreeResource with DEFINE_FREE_RESOURCE(TheClass); each class has its own was_used,
// resource.
#define DEFINE_FREE_RESOURCE(Class) \
void Class::FreeResource() \
{ \
    if (was_used) \
    { \
        delete resource; \
        was_used = false; \
    } \
}

#endif
