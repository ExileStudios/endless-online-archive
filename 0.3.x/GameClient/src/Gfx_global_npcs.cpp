#include <vcl.h>
#pragma hdrstop

#include "Gfx_global_npcs.h"
#include "Tmainform.h"
#include "Gfx_game.h"
#include "Gfx_interface.h"
#include "Gfx_mapnpcs.h"
#include "Gfx_npc.h"

#pragma package(smart_init)

NpcGlobalGraphics::NpcGlobalGraphics()
{
}

NpcGlobalGraphics::~NpcGlobalGraphics()
{
}

// Draws an npc at the point in its current frame, standing, walking, idling or attacking, mirrored for two of the
// four directions, then its damage number and health bar when shown.
bool NpcGlobalGraphics::DrawNpc(short x, short y, Npc npc, short x_offset, char alpha)
{
    NpcWrapper *graphic = MainForm->gfx->npc_cache->GetGraphic(npc.graphic);
    if (!graphic)
        return false;
    x += x_offset;
    frame_rect.left = x - graphic->resource->frame_width / 2;
    frame_rect.right = graphic->resource->frame_width + frame_rect.left;
    frame_rect.top = y - graphic->resource->frame_height;
    frame_rect.bottom = y;
    int label_x = x;
    int label_y = y - MainForm->npcs->head_heights[npc.graphic - 1];
    // Each graphic has its own frame offset from the tile; its x flips with the mirror.
    frame_rect.top -= MainForm->npcs->frame_offsets[npc.graphic - 1].y;
    frame_rect.bottom -= MainForm->npcs->frame_offsets[npc.graphic - 1].y;
    if (npc.direction > FacingDirection_Left)
    {
        mirrored = true;
        frame_rect.left -= MainForm->npcs->frame_offsets[npc.graphic - 1].x;
        frame_rect.right -= MainForm->npcs->frame_offsets[npc.graphic - 1].x;
    }
    else
    {
        mirrored = false;
        frame_rect.left += MainForm->npcs->frame_offsets[npc.graphic - 1].x;
        frame_rect.right += MainForm->npcs->frame_offsets[npc.graphic - 1].x;
    }
    if (npc.attacking && npc.attack_frame <= 3)
    {
        int attack_frame = npc.attack_frame + 6;
        // The first attack step shows the first attack frame.
        // Later steps show the second one, moved by the graphic's attack offset.
        if (attack_frame >= 7)
        {
            attack_frame = 7;
            if (npc.direction > FacingDirection_Left)
            {
                frame_rect.left -= MainForm->npcs->frame_offsets[npc.graphic + 239].x;
                frame_rect.right -= MainForm->npcs->frame_offsets[npc.graphic + 239].x;
                label_x -= MainForm->npcs->frame_offsets[npc.graphic + 239].x;
            }
            else
            {
                frame_rect.left += MainForm->npcs->frame_offsets[npc.graphic + 239].x;
                frame_rect.right += MainForm->npcs->frame_offsets[npc.graphic + 239].x;
                label_x += MainForm->npcs->frame_offsets[npc.graphic + 239].x;
            }
            if (npc.direction == FacingDirection_Down || npc.direction == FacingDirection_Right)
            {
                frame_rect.top -= MainForm->npcs->frame_offsets[npc.graphic + 239].y;
                frame_rect.bottom -= MainForm->npcs->frame_offsets[npc.graphic + 239].y;
                label_y -= MainForm->npcs->frame_offsets[npc.graphic + 239].y;
            }
            else
            {
                frame_rect.top += MainForm->npcs->frame_offsets[npc.graphic + 239].y;
                frame_rect.bottom += MainForm->npcs->frame_offsets[npc.graphic + 239].y;
                label_y += MainForm->npcs->frame_offsets[npc.graphic + 239].y;
            }
        }
        if (npc.direction == FacingDirection_Down || npc.direction == FacingDirection_Right)
        {
            if (mirrored)
                BlitNpcFrame(frame_rect, graphic->resource->front_mirrored,
                    graphic->resource->GetFrameRect(attack_frame), 255);
            else
                BlitNpcFrame(frame_rect, graphic->resource->front_sheet, graphic->resource->GetFrameRect(attack_frame),
                    255);
        }
        else if (mirrored)
            BlitNpcFrame(frame_rect, graphic->resource->back_mirrored, graphic->resource->GetFrameRect(attack_frame),
                255);
        else
            BlitNpcFrame(frame_rect, graphic->resource->back_sheet, graphic->resource->GetFrameRect(attack_frame), 255);
    }
    else if (npc.alt_attacking && npc.alt_attack_frame <= 2)
    {
        if (npc.direction == FacingDirection_Down || npc.direction == FacingDirection_Right)
        {
            if (mirrored)
                BlitNpcFrame(frame_rect, graphic->resource->front_mirrored, graphic->resource->GetFrameRect(8), 255);
            else
                BlitNpcFrame(frame_rect, graphic->resource->front_sheet, graphic->resource->GetFrameRect(8), 255);
        }
        else if (mirrored)
            BlitNpcFrame(frame_rect, graphic->resource->back_mirrored, graphic->resource->GetFrameRect(8), 255);
        else
            BlitNpcFrame(frame_rect, graphic->resource->back_sheet, graphic->resource->GetFrameRect(8), 255);
        label_x = -1;
    }
    else if (npc.walking)
    {
        if (npc.walk_direction == FacingDirection_Left)
        {
            frame_rect.left += 32 - (8 * npc.walk_frame + 8);
            frame_rect.top += 16 - (4 * npc.walk_frame + 4);
            frame_rect.right += 32 - (8 * npc.walk_frame + 8);
            frame_rect.bottom += 16 - (4 * npc.walk_frame + 4);
            label_x = label_x + 32 - (8 * npc.walk_frame + 8);
            label_y = label_y + 16 - (4 * npc.walk_frame + 4);
        }
        if (npc.walk_direction == FacingDirection_Up)
        {
            frame_rect.left = 8 * npc.walk_frame + frame_rect.left - 24;
            frame_rect.top -= 4 * npc.walk_frame - 12;
            frame_rect.right = 8 * npc.walk_frame + frame_rect.right - 24;
            frame_rect.bottom -= 4 * npc.walk_frame - 12;
            label_x = 8 * npc.walk_frame + label_x - 24;
            label_y = label_y + 16 - (4 * npc.walk_frame + 4);
        }
        if (npc.walk_direction == FacingDirection_Right)
        {
            frame_rect.left = 8 * npc.walk_frame + frame_rect.left - 24;
            frame_rect.top = 4 * npc.walk_frame + frame_rect.top - 12;
            frame_rect.right = 8 * npc.walk_frame + frame_rect.right - 24;
            frame_rect.bottom = 4 * npc.walk_frame + frame_rect.bottom - 12;
            label_x = 8 * npc.walk_frame + label_x - 24;
            label_y = 4 * npc.walk_frame + label_y - 12;
        }
        if (npc.walk_direction == FacingDirection_Down)
        {
            frame_rect.left += 32 - (8 * npc.walk_frame + 8);
            frame_rect.top -= 16 - (4 * npc.walk_frame + 4);
            frame_rect.right += 32 - (8 * npc.walk_frame + 8);
            frame_rect.bottom -= 16 - (4 * npc.walk_frame + 4);
            label_x = label_x + 32 - (8 * npc.walk_frame + 8);
            label_y = 4 * npc.walk_frame + label_y - 12;
        }
        if (npc.direction == FacingDirection_Down || npc.direction == FacingDirection_Right)
        {
            if (mirrored)
                BlitNpcFrame(frame_rect, graphic->resource->front_mirrored,
                    graphic->resource->GetFrameRect(npc.walk_frame + 2), alpha);
            else
                BlitNpcFrame(frame_rect, graphic->resource->front_sheet,
                    graphic->resource->GetFrameRect(npc.walk_frame + 2), alpha);
        }
        else if (mirrored)
            BlitNpcFrame(frame_rect, graphic->resource->back_mirrored,
                graphic->resource->GetFrameRect(npc.walk_frame + 2), alpha);
        else
            BlitNpcFrame(frame_rect, graphic->resource->back_sheet, graphic->resource->GetFrameRect(npc.walk_frame + 2),
                alpha);
    }
    else
    {
        unsigned char stand_frame = 0;
        // The idle counter shows the second stand frame on its last two steps.
        if (npc.idle_frame > 1)
            stand_frame = 1;
        if (npc.direction == FacingDirection_Down || npc.direction == FacingDirection_Right)
        {
            if (mirrored)
                BlitNpcFrame(frame_rect, graphic->resource->front_mirrored,
                    graphic->resource->GetFrameRect(stand_frame), alpha);
            else
                BlitNpcFrame(frame_rect, graphic->resource->front_sheet, graphic->resource->GetFrameRect(stand_frame),
                    alpha);
        }
        else if (mirrored)
            BlitNpcFrame(frame_rect, graphic->resource->back_mirrored, graphic->resource->GetFrameRect(stand_frame),
                alpha);
        else
            BlitNpcFrame(frame_rect, graphic->resource->back_sheet, graphic->resource->GetFrameRect(stand_frame),
                alpha);
    }
    if (npc.damage_shown)
        DrawDamageNumber(label_x, label_y, &npc);
    if (npc.health_bar_shown && label_x != 1)
    {
        if (!npc.health_bar_kind)
            DrawHealthBar(label_x, label_y, npc.health_bar_kind, npc.health_bar_width);
        if ((unsigned char)npc.health_bar_kind == 1)
            DrawHealthBar(label_x, label_y, npc.health_bar_kind, npc.second_bar_width);
        if ((unsigned char)npc.health_bar_kind == 2)
            DrawHealthBar(label_x, label_y, npc.health_bar_kind, npc.third_bar_width);
    }
    return true;
}

bool NpcGlobalGraphics::BlitNpcFrame(TRect dest_rect, TDx_DrawSurface *sheet, TRect source_rect, unsigned char alpha)
{
    if (alpha >= 255)
    {
        MainForm->gfx->game->BlitClipped(dest_rect.left, dest_rect.top, sheet, source_rect);
        return true;
    }
    MainForm->gfx->game->BlitClippedAlpha(dest_rect.left, dest_rect.top, sheet, source_rect, alpha);
    return true;
}

// Draws an npc's health bar at the point, filled by the level, its colour by the style and for style 0 by how low the
// level is; false for another style.
bool NpcGlobalGraphics::DrawHealthBar(int x, int y, unsigned char style, int level)
{
    MainForm->gfx->game->BlitClippedHalfBlend(x - 17, y - 52, MainForm->gfx->interface_graphics->hud_sheet,
        MainForm->gfx->interface_graphics->health_bar_shadow_rect);
    MainForm->gfx->game->BlitClipped(x - 17, y - 52, MainForm->gfx->interface_graphics->hud_sheet,
        MainForm->gfx->interface_graphics->health_bar_frame_rect);
    if (!style)
    {
        if ((short)level <= 5)
        {
            MainForm->gfx->game->BlitClipped(x - 15, y - 50, MainForm->gfx->interface_graphics->hud_sheet,
                MainForm->gfx->interface_graphics->GetHealthBarFillRect(HealthBarFill_Red, (short)level));
            return true;
        }
        if ((short)level > 5 && (short)level <= 12)
        {
            MainForm->gfx->game->BlitClipped(x - 15, y - 50, MainForm->gfx->interface_graphics->hud_sheet,
                MainForm->gfx->interface_graphics->GetHealthBarFillRect(HealthBarFill_Yellow, (short)level));
            return true;
        }
        MainForm->gfx->game->BlitClipped(x - 15, y - 50, MainForm->gfx->interface_graphics->hud_sheet,
            MainForm->gfx->interface_graphics->GetHealthBarFillRect(HealthBarFill_Green, (short)level));
        return true;
    }
    else if (style == 1)
    {
        MainForm->gfx->game->BlitClipped(x - 15, y - 50, MainForm->gfx->interface_graphics->hud_sheet,
            MainForm->gfx->interface_graphics->GetHealthBarFillRect(HealthBarFill_Second, (short)level));
        return true;
    }
    else if (style == 2)
    {
        MainForm->gfx->game->BlitClipped(x - 15, y - 50, MainForm->gfx->interface_graphics->hud_sheet,
            MainForm->gfx->interface_graphics->GetHealthBarFillRect(HealthBarFill_Third, (short)level));
        return true;
    }
    else
        return false;
}

// Draws the damage on an npc rising with its timer, one digit sprite per digit, or the miss label when it did none.
bool NpcGlobalGraphics::DrawDamageNumber(int x, int y, Npc *npc)
{
    if (npc->damage_amount <= 0 && !npc->damage_heal)
    {
        MainForm->gfx->game->BlitClipped(x - 13, 2 * npc->damage_timer + y - 81,
            MainForm->gfx->interface_graphics->hud_sheet,
            MainForm->gfx->interface_graphics->GetMissRect(npc->damage_heal));
        return true;
    }
    String digits = IntToStr(npc->damage_amount);
    for (int i = 1; i <= npc->damage_digits; i++)
    {
        int digit = StrToInt(digits[i]);
        MainForm->gfx->game->BlitClipped(x - 4 - 8 * npc->damage_digits / 2 + 8 * i, 2 * npc->damage_timer + y - 82,
            MainForm->gfx->interface_graphics->hud_sheet,
            MainForm->gfx->interface_graphics->GetDamageDigitRect(npc->damage_heal, digit));
    }
    return true;
}
