#include <vcl.h>
#pragma hdrstop

#include "Gfxvault.h"
#include "UnitMain.h"
#include <stdlib.h>

#pragma package(smart_init)

// Keeps the highest numeric bitmap ID seen so far; always goes on with the listing.
int __stdcall KeepHighestBitmapId(HINSTANCE__ *library, const char *resource_type, char *resource_name, long highest_id)
{
    if (((unsigned int)resource_name >> 16) == 0)
        *(int *)highest_id = max((int)resource_name, *(int *)highest_id);
    return 1;
}

GfxVault::GfxVault()
{
}

GfxVault::~GfxVault()
{
}

// Loads the item, npc, spell icon and spell effect gfx files.
void GfxVault::OpenLibraries()
{
    String path = FormMain->config->GetGfxPath() + "gfx023.egf";
    items_library = LoadLibraryA(path.c_str());
    path = FormMain->config->GetGfxPath() + "gfx021.egf";
    npcs_library = LoadLibraryA(path.c_str());
    path = FormMain->config->GetGfxPath() + "gfx024.egf";
    skills_library = LoadLibraryA(path.c_str());
    path = FormMain->config->GetGfxPath() + "gfx025.egf";
    icons_library = LoadLibraryA(path.c_str());
}

// Counts the graphics of each gfx file, from the number of bitmaps one graphic takes.
void GfxVault::CountGraphics()
{
    item_graphic_count = CountBitmaps(items_library);
    item_graphic_count = item_graphic_count - 100;
    item_graphic_count = item_graphic_count / 2;
    npc_graphic_count = CountBitmaps(npcs_library);
    npc_graphic_count = npc_graphic_count - 100;
    npc_graphic_count = npc_graphic_count / 40;
    skill_graphic_count = CountBitmaps(skills_library);
    skill_graphic_count = skill_graphic_count - 100;
    skill_graphic_count = skill_graphic_count / 3;
    icon_graphic_count = CountBitmaps(icons_library);
    icon_graphic_count = icon_graphic_count - 100;
}

// Frees the four gfx files.
void GfxVault::CloseLibraries()
{
    FreeLibrary(items_library);
    FreeLibrary(npcs_library);
    FreeLibrary(skills_library);
    FreeLibrary(icons_library);
}

// The highest bitmap ID in the gfx file.
int GfxVault::CountBitmaps(HINSTANCE__ *library)
{
    int count = 0;
    EnumResourceNamesA(library, RT_BITMAP,
        (int __stdcall (*)(HINSTANCE__ *, const char *, char *, long))KeepHighestBitmapId, (int)&count);
    return count;
}
