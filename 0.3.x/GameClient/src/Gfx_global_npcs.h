#ifndef Gfx_global_npcsH
#define Gfx_global_npcsH

#include <TDx_DrawSurface.H>

#include "Sys_npc.h"

struct NpcGlobalGraphics
{
    TRect frame_rect;
    bool mirrored;

    NpcGlobalGraphics();
    ~NpcGlobalGraphics();
    bool DrawNpc(short x, short y, Npc npc, short x_offset, char alpha);
    bool BlitNpcFrame(TRect dest_rect, TDx_DrawSurface *sheet, TRect source_rect, unsigned char alpha);
    bool DrawHealthBar(int x, int y, unsigned char style, int level);
    bool DrawDamageNumber(int x, int y, Npc *npc);
};

#endif
