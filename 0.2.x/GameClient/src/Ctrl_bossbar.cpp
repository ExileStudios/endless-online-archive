#include <vcl.h>
#pragma hdrstop

#include "Ctrl_bossbar.h"
#include "Tmainform.h"
#include "Gfx_game.h"
#include "Gfx_interface.h"
#include <time.h>

#pragma package(smart_init)

// A hidden boss bar and the surface its name is drawn on.
BossBar::BossBar()
{
    is_visible = false;
    needs_render = false;
    is_rendered = false;
    name_surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
    blt_fx = new TDDBltFX(NULL);
    blt_fx->FillColor = 0;
    name_rect.left = 0;
    name_rect.top = 0;
    name_rect.right = 190;
    name_rect.bottom = 14;
    surface_desc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    surface_desc->Width = 190;
    surface_desc->Height = 14;
    name_surface->GetSurfaceDesc(surface_desc);
    name_surface->Create(surface_desc, MainForm->Dx_Draw1);
    name_surface->SetColorKey(DDCKEY_SRCBLT, color_key);
    Hide();
}

BossBar::~BossBar()
{
    name_surface->Destroy();
    delete name_surface;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    color_key->Clear();
    blt_fx->Clear();
    delete color_key;
    delete blt_fx;
}

// Shows the bar for a new npc, starting it empty and timing it from now; nothing for the npc it shows already.
void BossBar::Show(int new_npc_index, int new_npc_id)
{
    if (new_npc_index != npc_index && new_npc_id != npc_id)
    {
        npc_index = new_npc_index;
        npc_id = new_npc_id;
        blink_frame = 0;
        has_health = false;
        is_blinking = false;
        needs_render = true;
        is_rendered = false;
        is_visible = true;
        damage_total = 0;
        shown_time = clock();
    }
}

// Hides the bar and frees it for another npc.
void BossBar::Hide()
{
    npc_index = -1;
    npc_id = -1;
    is_blinking = false;
    MainForm->redraw_needed = false;
    is_visible = false;
}

// Draws the boss's name on the bar's name surface once, with one rank icon per boss level up to two.
void BossBar::RenderName()
{
    HDC dc;
    if (needs_render)
    {
        NpcValue npc_value = MainForm->npc_values->GetNpcByIndex(npc_id);
        name_surface->Blt(&name_rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, blt_fx);
        rank_icons = 0;
        if (npc_value.boss >= 1)
            rank_icons++;
        if (npc_value.boss >= 2)
            rank_icons++;
        if (name_surface->GetDC(&dc))
        {
            try
            {
                MainForm->text_canvas->Handle = dc;
                MainForm->text_canvas->Brush->Style = bsClear;
                MainForm->text_canvas->Font->Color = clWhite;
                MainForm->text_canvas->Font->Name = "MS Sans Serif";
                MainForm->text_canvas->Font->Size = 7;
                MainForm->text_canvas->TextOut(0, 0, npc_value.name);
                MainForm->text_canvas->Handle = 0;
            }
            catch (Exception &)
            {
            }
            name_surface->ReleaseDC(dc);
        }
        MainForm->redraw_needed = true;
        needs_render = false;
        is_rendered = true;
    }
}

TRect BossBar::GetRankIconRect(int frame)
{
    TRect rect;
    rect.left = 9 * frame + 207;
    rect.right = 9 * frame + 216;
    rect.top = 0;
    rect.bottom = 9;
    return rect;
}

// Steps the bar's blink and hides it five seconds after it was shown.
void BossBar::Update()
{
    if (is_visible && is_rendered)
    {
        if (is_blinking)
        {
            if (!blink_counter)
            {
                blink_frame = 1;
                MainForm->redraw_needed = true;
            }
            if (blink_counter == 4)
            {
                blink_frame = 0;
                MainForm->redraw_needed = true;
            }
            blink_counter++;
            if (blink_counter > 8)
                blink_counter = 0;
        }
        if (clock() - shown_time > 5000)
            Hide();
    }
}

bool BossBar::IsVisible()
{
    return is_visible;
}

// Draws the bar: its back, the rank icons, the name fading as it blinks, and the health in one of three colours by
// what is left, or a full bar of its own colour before any health is known.
void BossBar::Draw(int x, int y, int offset_y)
{
    if (is_visible && is_rendered)
    {
        int top = y + offset_y;
        MainForm->gfx->game->BlitClippedHalfBlend(x - 9, top + 7, MainForm->gfx->interface_graphics->popup_icons,
            MainForm->gfx->interface_graphics->GetBarRowRect(0));
        for (int i = 0; i < rank_icons; i++)
            MainForm->OffscreenSurface->BltFast(10 * i + x + 3, top + 3, MainForm->gfx->interface_graphics->popup_icons,
                &GetRankIconRect(blink_frame), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        MainForm->gfx->game->BlitClippedAlpha(10 * rank_icons + x, top - 8, name_surface, name_rect,
            200 - 10 * blink_frame);
        if (!is_blinking && !has_health && health_percent <= 0)
            MainForm->OffscreenSurface->BltFast(x + 4, top + 17, MainForm->gfx->interface_graphics->popup_icons,
                &MainForm->gfx->interface_graphics->GetPercentBarRect(100, 3), DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(x + 4, top + 17, MainForm->gfx->interface_graphics->popup_icons,
                &MainForm->gfx->interface_graphics->GetPercentBarRect(health_percent, health_color), DDBLTFAST_WAIT);
    }
}
