#ifndef GfxvaultH
#define GfxvaultH

// The four gfx files the editor shows graphics from, and how many graphics each holds.
struct GfxVault
{
    HINSTANCE__ *items_library;
    HINSTANCE__ *npcs_library;
    HINSTANCE__ *skills_library;
    HINSTANCE__ *icons_library;
    int item_graphic_count;
    int npc_graphic_count;
    int skill_graphic_count;
    int icon_graphic_count;
    char unused_bytes[4];

    GfxVault();
    ~GfxVault();
    void OpenLibraries();
    void CountGraphics();
    void CloseLibraries();
    int CountBitmaps(HINSTANCE__ *library);
};

int __stdcall KeepHighestBitmapId(HINSTANCE__ *library, const char *resource_type, char *resource_name,
    long highest_id);

#endif
