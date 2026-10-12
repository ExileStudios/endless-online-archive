#include <vcl.h>
#pragma hdrstop

#include "Gfx_npcs.h"
#include "Tmainform.h"
#include "Gfx_interface.h"
#include "Gfx_npc.h"
#include "Gfx_game.h"
#pragma package(smart_init)

NpcGraphics::NpcGraphics()
{
    translucent_buffer = new TDx_DrawSurface(NULL);
    blt_fx = new TDDBltFX(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    surface_desc->Flags = 6;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = 2112;
    surface_desc->Width = 250;
    surface_desc->Height = 250;
    translucent_buffer->GetSurfaceDesc(surface_desc);
    translucent_buffer->Create(surface_desc, MainForm->Dx_Draw1);
    view_top = 8;
    view_left = 9;
    view_right = 629;
    view_bottom = 308;
}

NpcGraphics::~NpcGraphics()
{
    translucent_buffer->Destroy();
    delete translucent_buffer;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    blt_fx->Clear();
    delete blt_fx;
}

// Draws one npc on its tile: the frame of its current action, then its health bar and damage number.
// Frames of an npc sheet:
//   0-1  standing (the idle animation flips between them)
//   2-5  walking, one per walk step
//   6-7  attacking
//   8    the second animation
void NpcGraphics::DrawNpc(Npc npc, int draw_alpha, int x_offset)
{
    int attack_frame;
    if (draw_alpha < 250)
    {
        is_translucent = true;
        alpha = draw_alpha;
    }
    else
    {
        is_translucent = false;
        alpha = 255;
    }
    try
    {
        if (!npc.hidden)
        {
            // Tile to screen: the map is isometric, a tile is twice as wide as it is tall.
            int screen_x = TILE_HALF_WIDTH * npc.x - TILE_HALF_WIDTH * npc.y + MainForm->map->GetScrollOffsetX() + 319;
            int screen_y = 16 * npc.x + 16 * npc.y + MainForm->map->GetScrollOffsetY() + 182;
            if (x_offset)
                screen_x += x_offset;
            // The second pair of directions reuses the frames of the first pair, mirrored.
            if (npc.direction >= FacingDirection_Up)
            {
                blt_fx->FX = 2;
                is_mirrored = 1;
            }
            else
            {
                blt_fx->FX = 0;
                is_mirrored = 0;
            }
            char no_state_drawn = 1;
            char state_expired = 0;
            dest_rect.left = screen_x - npc.resources->frame_width / 2;
            dest_rect.right = dest_rect.left + npc.resources->frame_width;
            dest_rect.top = screen_y - npc.resources->frame_height;
            dest_rect.bottom = screen_y;
            // Each graphic has its own frame offset from the tile; its x flips with the mirror.
            dest_rect.top -= MainForm->npcs->frame_offsets[npc.graphic - 1].y;
            dest_rect.bottom -= MainForm->npcs->frame_offsets[npc.graphic - 1].y;
            if (npc.direction > FacingDirection_Left)
            {
                dest_rect.left -= MainForm->npcs->frame_offsets[npc.graphic - 1].x;
                dest_rect.right -= MainForm->npcs->frame_offsets[npc.graphic - 1].x;
            }
            else
            {
                dest_rect.left += MainForm->npcs->frame_offsets[npc.graphic - 1].x;
                dest_rect.right += MainForm->npcs->frame_offsets[npc.graphic - 1].x;
            }
            if (npc.attacking && no_state_drawn)
            {
                no_state_drawn = 0;
                if (npc.attack_frame <= 3)
                {
                    attack_frame = npc.attack_frame + 6;
                    // The first attack step shows the first attack frame.
                    // Later steps show the second one, moved by the graphic's attack offset.
                    if (attack_frame >= 7)
                    {
                        attack_frame = 7;
                        if (npc.direction > FacingDirection_Left)
                        {
                            dest_rect.left -= MainForm->npcs->attack_frame_offsets[npc.graphic - 1].x;
                            dest_rect.right -= MainForm->npcs->attack_frame_offsets[npc.graphic - 1].x;
                        }
                        else
                        {
                            dest_rect.left += MainForm->npcs->attack_frame_offsets[npc.graphic - 1].x;
                            dest_rect.right += MainForm->npcs->attack_frame_offsets[npc.graphic - 1].x;
                        }
                        if (npc.direction == FacingDirection_Down || npc.direction == FacingDirection_Right)
                        {
                            dest_rect.top -= MainForm->npcs->attack_frame_offsets[npc.graphic - 1].y;
                            dest_rect.bottom -= MainForm->npcs->attack_frame_offsets[npc.graphic - 1].y;
                        }
                        else
                        {
                            dest_rect.top += MainForm->npcs->attack_frame_offsets[npc.graphic - 1].y;
                            dest_rect.bottom += MainForm->npcs->attack_frame_offsets[npc.graphic - 1].y;
                        }
                    }
                    if (npc.direction == FacingDirection_Down || npc.direction == FacingDirection_Right)
                        BlitFrame(dest_rect, npc.resources->front_sheet,
                            Gfx_npc_GetFrameRect(npc.resources, attack_frame), true);
                    else
                        BlitFrame(dest_rect, npc.resources->back_sheet,
                            Gfx_npc_GetFrameRect(npc.resources, attack_frame), true);
                    if (draw_alpha)
                        DrawTranslucentBuffer(dest_rect);
                    if (npc.health_bar_shown)
                    {
                        int attack_bar_x;
                        int attack_bar_y = screen_y - MainForm->npcs->head_heights[npc.graphic - 1];
                        if (npc.direction > FacingDirection_Left)
                            attack_bar_x = screen_x - MainForm->npcs->attack_frame_offsets[npc.graphic - 1].x;
                        else
                            attack_bar_x = screen_x + MainForm->npcs->attack_frame_offsets[npc.graphic - 1].x;
                        if (npc.direction == FacingDirection_Down || npc.direction == FacingDirection_Right)
                            attack_bar_y = attack_bar_y - MainForm->npcs->attack_frame_offsets[npc.graphic - 1].y;
                        else
                            attack_bar_y = attack_bar_y + MainForm->npcs->attack_frame_offsets[npc.graphic - 1].y;
                        // Health bar: a half-blended shadow, the frame, then the fill.
                        // The fill's colour row follows its width.
                        MainForm->gfx->game->BlitClippedHalfBlend(attack_bar_x - 26, attack_bar_y - 60,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            MainForm->gfx->interface_graphics->health_bar_shadow_rect);
                        MainForm->gfx->game->BlitClipped(attack_bar_x - 26, attack_bar_y - 60,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            MainForm->gfx->interface_graphics->health_bar_frame_rect);
                        if (npc.health_bar_width <= 5)
                            MainForm->gfx->game->BlitClipped(attack_bar_x - 24, attack_bar_y - 58,
                                MainForm->gfx->interface_graphics->hud_sheet,
                                Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 2,
                                npc.health_bar_width));
                        if (npc.health_bar_width > 5 && npc.health_bar_width <= 12)
                            MainForm->gfx->game->BlitClipped(attack_bar_x - 24, attack_bar_y - 58,
                                MainForm->gfx->interface_graphics->hud_sheet,
                                Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 1,
                                npc.health_bar_width));
                        if (npc.health_bar_width > 12)
                            MainForm->gfx->game->BlitClipped(attack_bar_x - 24, attack_bar_y - 58,
                                MainForm->gfx->interface_graphics->hud_sheet,
                                Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 0,
                                npc.health_bar_width));
                    }
                    if (npc.damage_shown)
                    {
                        int attack_damage_y;
                        int attack_damage_x;
                        String attack_damage_text = IntToStr(npc.damage_amount);
                        attack_damage_y = screen_y - MainForm->npcs->head_heights[npc.graphic - 1];
                        if (npc.direction > FacingDirection_Left)
                            attack_damage_x = screen_x - MainForm->npcs->attack_frame_offsets[npc.graphic - 1].x;
                        else
                            attack_damage_x = screen_x + MainForm->npcs->attack_frame_offsets[npc.graphic - 1].x;
                        if (npc.direction == FacingDirection_Down || npc.direction == FacingDirection_Right)
                            attack_damage_y -= MainForm->npcs->attack_frame_offsets[npc.graphic - 1].y;
                        else
                            attack_damage_y += MainForm->npcs->attack_frame_offsets[npc.graphic - 1].y;
                        // A hit that does no damage shows the miss label, otherwise one centred digit sprite per digit.
                        // The number rises as its timer runs down.
                        if (npc.damage_amount <= 0 && !npc.damage_heal)
                            MainForm->gfx->game->BlitClipped(attack_damage_x - 21,
                                attack_damage_y + 2 * npc.damage_timer - 87,
                                MainForm->gfx->interface_graphics->hud_sheet,
                                MainForm->gfx->interface_graphics->miss_label_rect);
                        else
                        {
                            for (int i = 1; i <= npc.damage_digits; i++)
                            {
                                int attack_digit = StrToInt(String(attack_damage_text[i]));
                                if (!npc.damage_heal)
                                    MainForm->gfx->game->BlitClipped(attack_damage_x - 13 - 8 * npc.damage_digits / 2 +
                                        8 * i, attack_damage_y + 2 * npc.damage_timer - 87,
                                        MainForm->gfx->interface_graphics->hud_sheet,
                                        Gfx_interface_GetDamageDigitRect((int)MainForm->gfx->interface_graphics, 0,
                                        attack_digit));
                                else
                                    MainForm->gfx->game->BlitClipped(attack_damage_x - 13 - 8 * npc.damage_digits / 2 +
                                        8 * i, attack_damage_y + 2 * npc.damage_timer - 87,
                                        MainForm->gfx->interface_graphics->hud_sheet,
                                        Gfx_interface_GetDamageDigitRect((int)MainForm->gfx->interface_graphics, 1,
                                        attack_digit));
                            }
                        }
                    }
                }
                else
                    state_expired = 1;
            }
            if (npc.alt_attacking && no_state_drawn)
            {
                no_state_drawn = 0;
                if (npc.alt_attack_frame <= 2)
                {
                    if (npc.direction == FacingDirection_Down || npc.direction == FacingDirection_Right)
                        BlitFrame(dest_rect, npc.resources->front_sheet, Gfx_npc_GetFrameRect(npc.resources, 8), true);
                    else
                        BlitFrame(dest_rect, npc.resources->back_sheet, Gfx_npc_GetFrameRect(npc.resources, 8), true);
                    if (draw_alpha)
                        DrawTranslucentBuffer(dest_rect);
                }
                else
                    state_expired = 1;
            }
            if (npc.walking && no_state_drawn)
            {
                no_state_drawn = 0;
                // Walking: the frame starts back along the walk direction.
                // Each walk step brings it closer to the tile.
                if (npc.walk_direction == FacingDirection_Left)
                {
                    dest_rect.left += 32 - (8 * npc.walk_frame + 8);
                    dest_rect.top += 16 - (4 * npc.walk_frame + 4);
                    dest_rect.right += 32 - (8 * npc.walk_frame + 8);
                    dest_rect.bottom += 16 - (4 * npc.walk_frame + 4);
                }
                if (npc.walk_direction == FacingDirection_Up)
                {
                    dest_rect.left = 8 * npc.walk_frame + dest_rect.left - 24;
                    dest_rect.top -= 4 * npc.walk_frame - 12;
                    dest_rect.right = 8 * npc.walk_frame + dest_rect.right - 24;
                    dest_rect.bottom -= 4 * npc.walk_frame - 12;
                }
                if (npc.walk_direction == FacingDirection_Right)
                {
                    dest_rect.left = 8 * npc.walk_frame + dest_rect.left - 24;
                    dest_rect.top = 4 * npc.walk_frame + dest_rect.top - 12;
                    dest_rect.right = 8 * npc.walk_frame + dest_rect.right - 24;
                    dest_rect.bottom = 4 * npc.walk_frame + dest_rect.bottom - 12;
                }
                if (npc.walk_direction == FacingDirection_Down)
                {
                    dest_rect.left += 32 - (8 * npc.walk_frame + 8);
                    dest_rect.top -= 16 - (4 * npc.walk_frame + 4);
                    dest_rect.right += 32 - (8 * npc.walk_frame + 8);
                    dest_rect.bottom -= 16 - (4 * npc.walk_frame + 4);
                }
                if (npc.direction == FacingDirection_Down || npc.direction == FacingDirection_Right)
                    BlitFrame(dest_rect, npc.resources->front_sheet,
                        Gfx_npc_GetFrameRect(npc.resources, npc.walk_frame + 2), true);
                else
                    BlitFrame(dest_rect, npc.resources->back_sheet,
                        Gfx_npc_GetFrameRect(npc.resources, npc.walk_frame + 2), true);
                if (npc.health_bar_shown)
                {
                    int walk_bar_y;
                    int walk_bar_x;
                    if (npc.walk_direction == FacingDirection_Left)
                    {
                        walk_bar_x = screen_x + 32 - (8 * npc.walk_frame + 8);
                        walk_bar_y = screen_y + 16 - (4 * npc.walk_frame + 4);
                    }
                    if (npc.walk_direction == FacingDirection_Up)
                    {
                        walk_bar_x = screen_x + 8 * npc.walk_frame - 24;
                        walk_bar_y = screen_y + 16 - (4 * npc.walk_frame + 4);
                    }
                    if (npc.walk_direction == FacingDirection_Right)
                    {
                        walk_bar_x = screen_x + 8 * npc.walk_frame - 24;
                        walk_bar_y = screen_y + 4 * npc.walk_frame - 12;
                    }
                    if (npc.walk_direction == FacingDirection_Down)
                    {
                        walk_bar_x = screen_x + 32 - (8 * npc.walk_frame + 8);
                        walk_bar_y = screen_y + 4 * npc.walk_frame - 12;
                    }
                    walk_bar_y -= MainForm->npcs->head_heights[npc.graphic - 1];
                    MainForm->gfx->game->BlitClippedHalfBlend(walk_bar_x - 26, walk_bar_y - 60,
                        MainForm->gfx->interface_graphics->hud_sheet,
                        MainForm->gfx->interface_graphics->health_bar_shadow_rect);
                    MainForm->gfx->game->BlitClipped(walk_bar_x - 26, walk_bar_y - 60,
                        MainForm->gfx->interface_graphics->hud_sheet,
                        MainForm->gfx->interface_graphics->health_bar_frame_rect);
                    if (npc.health_bar_width <= 5)
                        MainForm->gfx->game->BlitClipped(walk_bar_x - 24, walk_bar_y - 58,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 2,
                            npc.health_bar_width));
                    if (npc.health_bar_width > 5 && npc.health_bar_width <= 12)
                        MainForm->gfx->game->BlitClipped(walk_bar_x - 24, walk_bar_y - 58,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 1,
                            npc.health_bar_width));
                    if (npc.health_bar_width > 12)
                        MainForm->gfx->game->BlitClipped(walk_bar_x - 24, walk_bar_y - 58,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 0,
                            npc.health_bar_width));
                }
                if (npc.damage_shown)
                {
                    int walk_damage_y;
                    int walk_damage_x;
                    String walk_damage_text = IntToStr(npc.damage_amount);
                    if (npc.walk_direction == FacingDirection_Left)
                    {
                        walk_damage_x = screen_x + 32 - (8 * npc.walk_frame + 8);
                        walk_damage_y = screen_y + 16 - (4 * npc.walk_frame + 4);
                    }
                    if (npc.walk_direction == FacingDirection_Up)
                    {
                        walk_damage_x = screen_x + 8 * npc.walk_frame - 24;
                        walk_damage_y = screen_y + 16 - (4 * npc.walk_frame + 4);
                    }
                    if (npc.walk_direction == FacingDirection_Right)
                    {
                        walk_damage_x = screen_x + 8 * npc.walk_frame - 24;
                        walk_damage_y = screen_y + 4 * npc.walk_frame - 12;
                    }
                    if (npc.walk_direction == FacingDirection_Down)
                    {
                        walk_damage_x = screen_x + 32 - (8 * npc.walk_frame + 8);
                        walk_damage_y = screen_y + 4 * npc.walk_frame - 12;
                    }
                    walk_damage_y -= MainForm->npcs->head_heights[npc.graphic - 1];
                    if (npc.damage_amount <= 0 && !npc.damage_heal)
                        MainForm->gfx->game->BlitClipped(walk_damage_x - 21, walk_damage_y + 2 * npc.damage_timer - 87,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            MainForm->gfx->interface_graphics->miss_label_rect);
                    else
                    {
                        for (int j = 1; j <= npc.damage_digits; j++)
                        {
                            int walk_digit = StrToInt(String(walk_damage_text[j]));
                            if (!npc.damage_heal)
                                MainForm->gfx->game->BlitClipped(walk_damage_x - 13 - 8 * npc.damage_digits / 2 + 8 * j,
                                    walk_damage_y + 2 * npc.damage_timer - 87,
                                    MainForm->gfx->interface_graphics->hud_sheet,
                                    Gfx_interface_GetDamageDigitRect((int)MainForm->gfx->interface_graphics, 0,
                                    walk_digit));
                            else
                                MainForm->gfx->game->BlitClipped(walk_damage_x - 13 - 8 * npc.damage_digits / 2 + 8 * j,
                                    walk_damage_y + 2 * npc.damage_timer - 87,
                                    MainForm->gfx->interface_graphics->hud_sheet,
                                    Gfx_interface_GetDamageDigitRect((int)MainForm->gfx->interface_graphics, 1,
                                    walk_digit));
                        }
                    }
                }
                if (draw_alpha)
                    DrawTranslucentBuffer(dest_rect);
            }
            if (no_state_drawn || state_expired)
            {
                int v51b = 0;
                // The idle counter shows the second stand frame on its last two steps.
                if (npc.idle_frame > 1)
                    v51b = 1;
                if (npc.direction == FacingDirection_Down || npc.direction == FacingDirection_Right)
                    BlitFrame(dest_rect, npc.resources->front_sheet, Gfx_npc_GetFrameRect(npc.resources, v51b), true);
                else
                    BlitFrame(dest_rect, npc.resources->back_sheet, Gfx_npc_GetFrameRect(npc.resources, v51b), true);
                if (npc.health_bar_shown)
                {
                    int stand_bar_y = screen_y - MainForm->npcs->head_heights[npc.graphic - 1];
                    MainForm->gfx->game->BlitClippedHalfBlend(screen_x - 26, stand_bar_y - 60,
                        MainForm->gfx->interface_graphics->hud_sheet,
                        MainForm->gfx->interface_graphics->health_bar_shadow_rect);
                    MainForm->gfx->game->BlitClipped(screen_x - 26, stand_bar_y - 60,
                        MainForm->gfx->interface_graphics->hud_sheet,
                        MainForm->gfx->interface_graphics->health_bar_frame_rect);
                    if (npc.health_bar_width <= 5)
                        MainForm->gfx->game->BlitClipped(screen_x - 24, stand_bar_y - 58,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 2,
                            npc.health_bar_width));
                    if (npc.health_bar_width > 5 && npc.health_bar_width <= 12)
                        MainForm->gfx->game->BlitClipped(screen_x - 24, stand_bar_y - 58,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 1,
                            npc.health_bar_width));
                    if (npc.health_bar_width > 12)
                        MainForm->gfx->game->BlitClipped(screen_x - 24, stand_bar_y - 58,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 0,
                            npc.health_bar_width));
                }
                if (npc.damage_shown)
                {
                    int stand_damage_y;
                    String stand_damage_text = IntToStr(npc.damage_amount);
                    stand_damage_y = screen_y - MainForm->npcs->head_heights[npc.graphic - 1];
                    if (npc.damage_amount <= 0 && !npc.damage_heal)
                        MainForm->gfx->game->BlitClipped(screen_x - 21, stand_damage_y + 2 * npc.damage_timer - 87,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            MainForm->gfx->interface_graphics->miss_label_rect);
                    else
                    {
                        for (int k = 1; k <= npc.damage_digits; k++)
                        {
                            int stand_digit = StrToInt(String(stand_damage_text[k]));
                            if (!npc.damage_heal)
                                MainForm->gfx->game->BlitClipped(screen_x - 13 - 8 * npc.damage_digits / 2 + 8 * k,
                                    stand_damage_y + 2 * npc.damage_timer - 87,
                                    MainForm->gfx->interface_graphics->hud_sheet,
                                    Gfx_interface_GetDamageDigitRect((int)MainForm->gfx->interface_graphics, 0,
                                    stand_digit));
                            else
                                MainForm->gfx->game->BlitClipped(screen_x - 13 - 8 * npc.damage_digits / 2 + 8 * k,
                                    stand_damage_y + 2 * npc.damage_timer - 87,
                                    MainForm->gfx->interface_graphics->hud_sheet,
                                    Gfx_interface_GetDamageDigitRect((int)MainForm->gfx->interface_graphics, 1,
                                    stand_digit));
                        }
                    }
                }
                if (draw_alpha)
                    DrawTranslucentBuffer(dest_rect);
            }
        }
    }
    catch (...)
    {
    }
}

// Blits one frame of a sheet into the view, clipped to it; a translucent npc is blitted into the buffer instead.
void NpcGraphics::BlitFrame(Windows::TRect screen_rect, TDx_DrawSurface *sheet, Windows::TRect source_rect, bool keyed)
{
    int dest_width;
    int dest_height;
    char result = 1;
    dest_width = screen_rect.right - screen_rect.left;
    dest_height = screen_rect.bottom - screen_rect.top;
    if (screen_rect.left + dest_width < view_left)
        result = 0;
    if (screen_rect.top + dest_height < view_top)
        result = 0;
    if (screen_rect.left > view_right)
        result = 0;
    if (screen_rect.top > view_bottom)
        result = 0;
    if (result)
    {
        TRect clipped_dest = ClipToView(this, screen_rect);
        screen_rect = clipped_dest;
        if (is_translucent)
        {
            TRect buffer_rect;
            TRect buffer_source_rect;
            TRect clipped_source = GetClippedSourceRect(this, source_rect);
            buffer_source_rect = clipped_source;
            buffer_width = buffer_source_rect.right - buffer_source_rect.left;
            buffer_height = buffer_source_rect.bottom - buffer_source_rect.top;
            if (buffer_width < 0)
                buffer_width = 0;
            if (buffer_height < 0)
                buffer_height = 0;
            buffer_rect.right = buffer_width;
            buffer_rect.bottom = buffer_height;
            buffer_rect.left = 0;
            buffer_rect.top = 0;
            translucent_buffer->Blt(&buffer_rect, sheet, &buffer_source_rect, 16779264, blt_fx);
        }
        else
        {
            if (keyed)
            {
                TRect keyed_source_rect = GetClippedSourceRect(this, source_rect);
                MainForm->OffscreenSurface->Blt(&screen_rect, sheet, &keyed_source_rect, 16812032, blt_fx);
            }
            else
            {
                TRect plain_source_rect = GetClippedSourceRect(this, source_rect);
                MainForm->OffscreenSurface->Blt(&screen_rect, sheet, &plain_source_rect, 16779264, blt_fx);
            }
        }
    }
}

// Blends the frame held in the translucent buffer onto the view at the npc's alpha.
void NpcGraphics::DrawTranslucentBuffer(Windows::TRect screen_rect)
{
    int dest_width;
    int dest_height;
    char result = 1;
    dest_width = screen_rect.right - screen_rect.left;
    dest_height = screen_rect.bottom - screen_rect.top;
    if (screen_rect.left + dest_width < view_left)
        result = 0;
    if (screen_rect.top + dest_height < view_top)
        result = 0;
    if (screen_rect.left > view_right)
        result = 0;
    if (screen_rect.top > view_bottom)
        result = 0;
    if (result)
    {
        if (is_translucent)
        {
            TRect clipped_dest = ClipToView(this, screen_rect);
            screen_rect = clipped_dest;
            Windows::TRect buffer_rect;
            buffer_rect.left = 0;
            buffer_rect.top = 0;
            buffer_rect.right = buffer_width;
            buffer_rect.bottom = buffer_height;
            // The alpha blit counts from the view's corner.
            MainForm->gfx->game->BlitClippedAlpha(screen_rect.left - 9, screen_rect.top - 8, translucent_buffer,
                buffer_rect, alpha);
            return;
        }
    }
}

// Cuts a rect to the view and keeps how much came off each side, for GetClippedSourceRect.
DEFINE_CLIP_TO_VIEW(NpcGraphics)

// Cuts the same amounts off the source frame; on a mirrored frame the left and right cuts trade places.
DEFINE_GET_CLIPPED_SOURCE_RECT(NpcGraphics)

// Wraps an npc's text into at most six balloon lines, measures them and hands them to the npc list.
void NpcGraphics::ShowSpeechBalloon(String text, int npc_index, int style)
{
    int text_width;
    int text_height;
    int next_line_start;
    HDC dc;
    if (MainForm->config->GetShowBalloons())
    {
        int wrap_width = 150;
        text_width = 0;
        text_height = 0;
        String trimmed_text = text.Trim();
        next_line_start = 1;
        String balloon_lines[6];
        for (int i = 0; i <= 5; i++)
            balloon_lines[i] = "";
        if (MainForm->OffscreenSurface->GetDC(&dc))
        {
            try
            {
                MainForm->draw_canvas->Handle = dc;
                MainForm->draw_canvas->Brush->Color = clBlack;
                MainForm->draw_canvas->Brush->Style = bsClear;
                MainForm->draw_canvas->Font->Color = (TColor)0x80808;
                MainForm->draw_canvas->Font->Name = "MS Sans Serif";
                MainForm->draw_canvas->Font->Size = 8;
                for (int line_index = 0; line_index <= 5; line_index++)
                {
                    if (text.Length() < next_line_start)
                        continue;
                    int line_start;
                    line_start = next_line_start;
                    char seeking_break = 1;
                    for (int j = next_line_start; j <= text.Length() && seeking_break; j++)
                    {
                        // A line breaks at a space once it is near the wrap width.
                        // At the width itself it breaks mid-word and gets a hyphen.
                        if (!((text[j] == ' ' &&
                            MainForm->draw_canvas->TextWidth(text.SubString(line_start, j - line_start)) >=
                            wrap_width - 50) ||
                            MainForm->draw_canvas->TextWidth(text.SubString(line_start, j - line_start)) >=
                            wrap_width || j == text.Length()))
                            continue;
                        if (MainForm->draw_canvas->TextWidth(text.SubString(line_start, j - line_start)) <=
                            wrap_width || j == text.Length())
                        {
                            if (MainForm->draw_canvas->TextWidth(text.SubString(line_start, j - line_start)) >
                                text_width - 4)
                                text_width = MainForm->draw_canvas->TextWidth(text.SubString(line_start,
                                    j - line_start)) + 4;
                            balloon_lines[line_index] = text.SubString(line_start, j - line_start + 1);
                            text_height = text_height + MainForm->draw_canvas->TextHeight(text) + 1;
                            next_line_start = j + 1;
                            seeking_break = 0;
                        }
                        else
                        {
                            if (MainForm->draw_canvas->TextWidth(text.SubString(line_start, j - line_start) + "-") >
                                text_width - 4)
                                text_width = MainForm->draw_canvas->TextWidth(text.SubString(line_start,
                                    j - line_start) + "-") + 4;
                            balloon_lines[line_index] = text.SubString(line_start, j - line_start + 1) + "-";
                            text_height = text_height + MainForm->draw_canvas->TextHeight(text) + 1;
                            next_line_start = j + 1;
                            seeking_break = 0;
                        }
                    }
                }
                MainForm->draw_canvas->Handle = 0;
            }
            catch (Exception &)
            {
            }
            MainForm->OffscreenSurface->ReleaseDC(dc);
        }
        MainForm->npcs->ShowBalloon(npc_index, balloon_lines, text_width, text_height, style);
    }
}
