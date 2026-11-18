#include <vcl.h>
#pragma hdrstop

#include "Gfx_global_sprites.h"
#include "Tmainform.h"
#include "Ctrl_interface.h"
#include "Gfx_emotebuffer.h"
#include "Gfx_game.h"
#include "Gfx_interface.h"
#include "Gfx_player.h"
#include "Gfx_playername.h"

#pragma package(smart_init)

// Creates the character sheets at their fixed sizes and sets the view bounds the sprites are clipped to.
SpriteGlobalGraphics::SpriteGlobalGraphics()
{
    stand_sheet = new TDx_DrawSurface(NULL);
    walk_sheet = new TDx_DrawSurface(NULL);
    attack_sheet = new TDx_DrawSurface(NULL);
    spell_cast_sheet = new TDx_DrawSurface(NULL);
    chair_sit_sheet = new TDx_DrawSurface(NULL);
    floor_sit_sheet = new TDx_DrawSurface(NULL);
    ranged_attack_sheet = new TDx_DrawSurface(NULL);
    face_sheet = new TDx_DrawSurface(NULL);
    translucent_buffer = new TDx_DrawSurface(NULL);
    kind_sheet = new TDx_DrawSurface(NULL);
    blt_fx = new TDDBltFX(NULL);
    view_top = 8;
    view_left = 9;
    view_right = 629;
    view_bottom = 308;
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    surface_desc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    surface_desc->Width = 72;
    surface_desc->Height = 696;
    stand_sheet->GetSurfaceDesc(surface_desc);
    stand_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 416;
    surface_desc->Height = 732;
    walk_sheet->GetSurfaceDesc(surface_desc);
    walk_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 192;
    surface_desc->Height = 696;
    attack_sheet->GetSurfaceDesc(surface_desc);
    attack_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 72;
    surface_desc->Height = 744;
    spell_cast_sheet->GetSurfaceDesc(surface_desc);
    spell_cast_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 96;
    surface_desc->Height = 624;
    chair_sit_sheet->GetSurfaceDesc(surface_desc);
    chair_sit_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 96;
    surface_desc->Height = 516;
    floor_sit_sheet->GetSurfaceDesc(surface_desc);
    floor_sit_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 100;
    surface_desc->Height = 696;
    ranged_attack_sheet->GetSurfaceDesc(surface_desc);
    ranged_attack_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 143;
    surface_desc->Height = 336;
    face_sheet->GetSurfaceDesc(surface_desc);
    face_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 68;
    surface_desc->Height = 84;
    translucent_buffer->GetSurfaceDesc(surface_desc);
    translucent_buffer->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 96;
    surface_desc->Height = 129;
    kind_sheet->GetSurfaceDesc(surface_desc);
    kind_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    emote_buffer_created = false;
}

SpriteGlobalGraphics::~SpriteGlobalGraphics()
{
    stand_sheet->Destroy();
    walk_sheet->Destroy();
    attack_sheet->Destroy();
    spell_cast_sheet->Destroy();
    chair_sit_sheet->Destroy();
    floor_sit_sheet->Destroy();
    ranged_attack_sheet->Destroy();
    face_sheet->Destroy();
    translucent_buffer->Destroy();
    kind_sheet->Destroy();
    delete stand_sheet;
    delete walk_sheet;
    delete attack_sheet;
    delete spell_cast_sheet;
    delete chair_sit_sheet;
    delete floor_sit_sheet;
    delete ranged_attack_sheet;
    delete face_sheet;
    delete translucent_buffer;
    delete kind_sheet;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    blt_fx->Clear();
    delete blt_fx;
}

// The place of a standing frame on the sheet by gender, facing and skin; facings 2 and 3 reuse those of 1 and 0.
TRect SpriteGlobalGraphics::GetStandFrameRect(int gender, int facing, int skin)
{
    TRect rect;
    if (facing >= 2)
    {
        if (facing == 2)
            facing = 1;
        else
            facing = 0;
    }
    if (skin > 11)
        skin = 11;
    rect.left = 36 * gender + 18 * facing;
    rect.top = 58 * skin;
    rect.right = 36 * gender + 18 * facing + 18;
    rect.bottom = 58 * skin + 58;
    return rect;
}

// The place of a walking frame on the sheet by gender, facing, skin and step 0 to 3; facings 2 and 3 reuse 1 and 0.
TRect SpriteGlobalGraphics::GetWalkFrameRect(int gender, int facing, int skin, int frame)
{
    TRect rect;
    if (facing >= 2)
    {
        if (facing == 2)
            facing = 1;
        else
            facing = 0;
    }
    if (frame < 0)
        frame = 0;
    if (frame > 3)
        frame = 3;
    rect.left = 26 * frame + 208 * gender + 104 * facing;
    rect.top = 61 * skin;
    rect.right = 26 * frame + 208 * gender + 104 * facing + 26;
    rect.bottom = 61 * skin + 61;
    return rect;
}

// The place of an attack frame on the sheet by gender, facing, skin and frame 0 or 1; facings 2 and 3 reuse 1 and 0.
TRect SpriteGlobalGraphics::GetAttackFrameRect(int gender, int facing, int skin, int frame)
{
    TRect rect;
    if (facing >= 2)
    {
        if (facing == 2)
            facing = 1;
        else
            facing = 0;
    }
    if (frame < 0)
        frame = 0;
    if (frame > 1)
        frame = 1;
    rect.left = 24 * frame + 96 * gender + 48 * facing;
    rect.top = 58 * skin;
    rect.right = 24 * frame + 96 * gender + 48 * facing + 24;
    rect.bottom = 58 * skin + 58;
    return rect;
}

// The place of a spell casting frame on the sheet by gender, facing and skin; facings 2 and 3 reuse 1 and 0.
TRect SpriteGlobalGraphics::GetSpellCastCellRect(int gender, int facing, int skin)
{
    TRect rect;
    if (facing >= 2)
    {
        if (facing == 2)
            facing = 1;
        else
            facing = 0;
    }
    rect.left = 36 * gender + 18 * facing;
    rect.top = 62 * skin;
    rect.right = 36 * gender + 18 * facing + 18;
    rect.bottom = 62 * skin + 62;
    return rect;
}

// The place of a frame sitting on a chair on the sheet by gender, facing and skin; facings 2 and 3 reuse 1 and 0.
TRect SpriteGlobalGraphics::GetChairSitFrameRect(int gender, int facing, int skin)
{
    TRect rect;
    if (facing >= 2)
    {
        if (facing == 2)
            facing = 1;
        else
            facing = 0;
    }
    rect.left = 48 * gender + 24 * facing;
    rect.top = 52 * skin;
    rect.right = 48 * gender + 24 * facing + 24;
    rect.bottom = 52 * skin + 52;
    return rect;
}

// The place of a frame sitting on the floor on the sheet by gender, facing and skin; facings 2 and 3 reuse 1 and 0.
TRect SpriteGlobalGraphics::GetFloorSitFrameRect(int gender, int facing, int skin)
{
    TRect rect;
    if (facing >= 2)
    {
        if (facing == 2)
            facing = 1;
        else
            facing = 0;
    }
    rect.left = 48 * gender + 24 * facing;
    rect.top = 43 * skin;
    rect.right = 48 * gender + 24 * facing + 24;
    rect.bottom = 43 * skin + 43;
    return rect;
}

// The place of a ranged attack frame on the sheet by gender, facing and skin; facings 2 and 3 reuse 1 and 0.
TRect SpriteGlobalGraphics::GetRangedAttackFrameRect(int gender, int facing, int skin)
{
    TRect rect;
    if (facing >= 2)
    {
        if (facing == 2)
            facing = 1;
        else
            facing = 0;
    }
    rect.left = 50 * gender + 25 * facing;
    rect.top = 58 * skin;
    rect.right = 50 * gender + 25 * facing + 25;
    rect.bottom = 58 * skin + 58;
    return rect;
}

TRect SpriteGlobalGraphics::GetFaceStripRect(int gender, int skin)
{
    TRect rect;
    rect.left = 0;
    rect.right = 143;
    rect.top = 168 * gender + 14 * skin;
    rect.bottom = 168 * gender + 14 * skin + 14;
    return rect;
}

// The place of the frame in the column, in the row of the kind: kind 9 takes the second row, 11 and up the third.
TRect SpriteGlobalGraphics::GetKindFrameRect(int unused, int column, int kind)
{
    TRect rect;
    unsigned char row = 0;
    if (kind == 9)
        row = 1;
    else if (kind >= 11)
        row = 2;
    rect.left = 24 * column;
    rect.top = 43 * row;
    rect.right = 24 * column + 24;
    rect.bottom = 43 * row + 43;
    return rect;
}

// The head of the gender and skin on the sprite sheet; skins past 11 are drawn as 11.
TRect SpriteGlobalGraphics::GetHeadRect(int gender, int skin)
{
    TRect rect;
    if (skin > 11)
        skin = 11;
    rect.left = 36 * gender;
    rect.top = 58 * skin;
    rect.right = 36 * gender + 15;
    rect.bottom = 58 * skin + 16;
    return rect;
}

// The head with its neck, 6 pixels taller for gender 0 and 4 for the other.
TRect SpriteGlobalGraphics::GetHeadAndNeckRect(int gender, int skin)
{
    TRect rect = GetHeadRect(gender, skin);
    if (!gender)
        rect.bottom += 6;
    else
        rect.bottom += 4;
    return rect;
}

void SpriteGlobalGraphics::LoadStandSheet(int library, int resource_id)
{
    LoadSheet(stand_sheet, library, resource_id);
    stand_sheet->SetColorKey(DDCKEY_SRCBLT, color_key);
}

void SpriteGlobalGraphics::LoadWalkSheet(int library, int resource_id)
{
    LoadSheet(walk_sheet, library, resource_id);
    walk_sheet->SetColorKey(DDCKEY_SRCBLT, color_key);
}

void SpriteGlobalGraphics::LoadAttackSheet(int library, int resource_id)
{
    LoadSheet(attack_sheet, library, resource_id);
    attack_sheet->SetColorKey(DDCKEY_SRCBLT, color_key);
}

void SpriteGlobalGraphics::LoadSpellCastSheet(int library, int resource_id)
{
    LoadSheet(spell_cast_sheet, library, resource_id);
    spell_cast_sheet->SetColorKey(DDCKEY_SRCBLT, color_key);
}

void SpriteGlobalGraphics::LoadChairSitSheet(int library, int resource_id)
{
    LoadSheet(chair_sit_sheet, library, resource_id);
    chair_sit_sheet->SetColorKey(DDCKEY_SRCBLT, color_key);
}

void SpriteGlobalGraphics::LoadFloorSitSheet(int library, int resource_id)
{
    LoadSheet(floor_sit_sheet, library, resource_id);
    floor_sit_sheet->SetColorKey(DDCKEY_SRCBLT, color_key);
}

void SpriteGlobalGraphics::LoadRangedAttackSheet(int library, int resource_id)
{
    LoadSheet(ranged_attack_sheet, library, resource_id);
    ranged_attack_sheet->SetColorKey(DDCKEY_SRCBLT, color_key);
}

void SpriteGlobalGraphics::LoadFaceSheet(int library, int resource_id)
{
    LoadSheet(face_sheet, library, resource_id);
    face_sheet->SetColorKey(DDCKEY_SRCBLT, color_key);
}

void SpriteGlobalGraphics::LoadKindSheet(int library, int resource_id)
{
    LoadSheet(kind_sheet, library, resource_id);
    kind_sheet->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Loads the library's bitmap onto the sheet; false when it cannot.
bool SpriteGlobalGraphics::LoadSheet(TDx_DrawSurface *sheet, int library, int resource_id)
{
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id))
        return false;
    else
    {
        TPoint size = *(TPoint *)MainForm->gfx_transfers;
        if (MainForm->gfx_transfers->BeginDraw(sheet))
        {
            MainForm->gfx_transfers->Draw(sheet);
            MainForm->gfx_transfers->EndDraw(sheet);
            return true;
        }
        return false;
    }
}

// Draws a picture from the library onto the emote buffer at the offset, black as transparent; false when it cannot.
bool SpriteGlobalGraphics::DrawLayerToEmoteBuffer(int x_offset, int y_offset, int library, int resource_id)
{
    if (!emote_buffer_created)
        return false;
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id))
        return false;
    else
    {
        TPoint size = *(TPoint *)MainForm->gfx_transfers;
        MainForm->gfx_transfers->SetTransparentColor(clBlack);
        if (MainForm->gfx_transfers->BeginDraw(emote_buffer->surface))
        {
            MainForm->gfx_transfers->DrawAt(emote_buffer->surface, 0 - x_offset, 0 - y_offset);
            MainForm->gfx_transfers->EndDraw(emote_buffer->surface);
            return true;
        }
        return false;
    }
}

// Draws a picture from the library onto the emote buffer at the offset, the colour of one of its pixels as
// transparent; false when it cannot.
bool SpriteGlobalGraphics::DrawLayerToEmoteBufferPixelKeyed(int x_offset, int y_offset, int library, int resource_id)
{
    if (!emote_buffer_created)
        return false;
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id))
        return false;
    else
    {
        TPoint size = *(TPoint *)MainForm->gfx_transfers;
        MainForm->gfx_transfers->SetTransparentPixel(1, 32);
        if (MainForm->gfx_transfers->BeginDraw(emote_buffer->surface))
        {
            MainForm->gfx_transfers->DrawAt(emote_buffer->surface, 0 - x_offset, 0 - y_offset);
            MainForm->gfx_transfers->EndDraw(emote_buffer->surface);
            return true;
        }
        return false;
    }
}

// Draws the player's standing frame at the point, from the front or the back sheet by direction, mirrored for the
// directions from 2 up.
void SpriteGlobalGraphics::DrawPlayerAt(Player player, int hair_style, int hair_color, int x, int y)
{
    int draw_x = x;
    int draw_y = y;
    if (player.direction >= FacingDirection_Up)
    {
        blt_fx->FX = 2;
        is_mirrored = true;
    }
    else
    {
        blt_fx->FX = 0;
        is_mirrored = false;
    }
    player_rect.left = draw_x - 34;
    player_rect.right = draw_x + 34;
    player_rect.top = draw_y - 18;
    player_rect.bottom = draw_y + 66;
    if (player.direction > FacingDirection_Left)
    {
        player_rect.left += 18;
        player_rect.right += 18;
    }
    int facing = player.direction;
    if (facing == FacingDirection_Down || facing == FacingDirection_Right)
        BlitFrame(player_rect, player.resources->front_sheet, player.resources->GetFrameRect(0), false, true, &player);
    else
        BlitFrame(player_rect, player.resources->back_sheet, player.resources->GetFrameRect(0), false, true, &player);
}

// Draws a player in the current frame of what it does, with its equipment layers in the order its facing needs, then
// its health bar, the damage or miss it took and its emote.
void SpriteGlobalGraphics::DrawPlayer(Player *player, char draw_buffered, int x_offset)
{
    if (player->graphics_pending)
        draw_progress = player->load_progress;
    else
        draw_progress = 99;
    draw_progress = 99;
    use_frame_buffer = draw_buffered;
    emote_pass = false;
    emote_lowered = false;
    try
    {
        // Players of a preview group other than 0 are not drawn on the map.
        if (!player->draw_group)
        {
            int screen_x = 32 * player->x - 32 * player->y + MainForm->map->GetScrollOffsetX() + 310;
            int screen_y = 16 * player->x + 16 * player->y + MainForm->map->GetScrollOffsetY() + 102;
            if (x_offset)
                screen_x += x_offset;
            if (player->direction >= FacingDirection_Up)
            {
                blt_fx->FX = 2;
                is_mirrored = true;
            }
            else
            {
                blt_fx->FX = 0;
                is_mirrored = false;
            }
            char idle = true;
            char draw_standing = false;
            if (player->attacking && idle)
            {
                idle = false;
                // A ranged attack shows its frame on the middle two steps; the other steps draw standing.
                if (MainForm->weapon_mapper->IsRangedWeapon(player->weapon))
                {
                    if (player->attack_frame == 1 || player->attack_frame == 2)
                    {
                        if (player->attack_direction <= FacingDirection_Left)
                        {
                            player_rect.left = screen_x - 40;
                            player_rect.right = screen_x + 28;
                        }
                        else
                        {
                            player_rect.left = screen_x - 28;
                            player_rect.right = screen_x + 40;
                        }
                        if (player->attack_direction == FacingDirection_Down ||
                            player->attack_direction == FacingDirection_Right)
                        {
                            player_rect.top = screen_y - 17;
                            player_rect.bottom = screen_y + 67;
                        }
                        else
                        {
                            player_rect.top = screen_y - 19;
                            player_rect.bottom = screen_y + 65;
                        }
                        if (player->attack_direction == FacingDirection_Left ||
                            player->attack_direction == FacingDirection_Up)
                        {
                            player_rect.top++;
                            player_rect.bottom++;
                            if (player->attack_direction == FacingDirection_Left)
                            {
                                player_rect.left += 2;
                                player_rect.right += 2;
                            }
                            if (player->attack_direction == FacingDirection_Up)
                            {
                                player_rect.left = player_rect.left - 2;
                                player_rect.right = player_rect.right - 2;
                            }
                        }
                        if (player->attack_direction > FacingDirection_Left)
                        {
                            player_rect.left += 18;
                            player_rect.right += 18;
                        }
                        if (player->attack_direction == FacingDirection_Down ||
                            player->attack_direction == FacingDirection_Right)
                            BlitFrame(player_rect, player->resources->front_sheet, player->resources->GetFrameRect(10),
                                true, true, player);
                        else
                            BlitFrame(player_rect, player->resources->back_sheet, player->resources->GetFrameRect(10),
                                true, true, player);
                        if (draw_buffered)
                            DrawTranslucentBuffer(player_rect);
                    }
                    else
                        draw_standing = true;
                }
                else if (player->attack_frame <= 3)
                {
                    // A melee attack shows its first frame on step 0 and its second frame from step 1 on.
                    int attack_index = player->attack_frame + 5;
                    if (attack_index >= 6)
                    {
                        attack_index = 6;
                        if (player->attack_direction <= FacingDirection_Left)
                        {
                            player_rect.left = screen_x - 40;
                            player_rect.right = screen_x + 28;
                        }
                        else
                        {
                            player_rect.left = screen_x - 28;
                            player_rect.right = screen_x + 40;
                        }
                        if (player->attack_direction == FacingDirection_Down ||
                            player->attack_direction == FacingDirection_Right)
                        {
                            player_rect.top = screen_y - 17;
                            player_rect.bottom = screen_y + 67;
                        }
                        else
                        {
                            player_rect.top = screen_y - 19;
                            player_rect.bottom = screen_y + 65;
                        }
                    }
                    else
                    {
                        player_rect.left = screen_x - 34;
                        player_rect.right = screen_x + 34;
                        player_rect.top = screen_y - 18;
                        player_rect.bottom = screen_y + 66;
                    }
                    if (player->attack_direction > FacingDirection_Left)
                    {
                        player_rect.left += 18;
                        player_rect.right += 18;
                    }
                    if (player->attack_direction == FacingDirection_Down ||
                        player->attack_direction == FacingDirection_Right)
                        BlitFrame(player_rect, player->resources->front_sheet,
                            player->resources->GetFrameRect(attack_index), true, true, player);
                    else
                        BlitFrame(player_rect, player->resources->back_sheet,
                            player->resources->GetFrameRect(attack_index), true, true, player);
                    if (draw_buffered)
                        DrawTranslucentBuffer(player_rect);
                }
                else
                    draw_standing = true;
                if (player->health_bar_shown)
                {
                    int attack_bar_x = screen_x;
                    int attack_bar_y = screen_y;
                    if (MainForm->weapon_mapper->IsRangedWeapon(player->weapon))
                    {
                        if (player->attack_frame == 1 || player->attack_frame == 2)
                        {
                            if (player->attack_direction <= FacingDirection_Left)
                                attack_bar_x = screen_x - 2;
                            else
                                attack_bar_x = screen_x + 2;
                            if (player->attack_direction == FacingDirection_Down ||
                                player->attack_direction == FacingDirection_Right)
                                attack_bar_y = screen_y + 1;
                        }
                    }
                    else if (player->attack_frame == 1 || player->attack_frame == 2 || player->attack_frame == 3)
                    {
                        if (player->attack_direction <= FacingDirection_Left)
                            attack_bar_x = screen_x - 6;
                        else
                            attack_bar_x = screen_x + 6;
                        if (player->attack_direction == FacingDirection_Down ||
                            player->attack_direction == FacingDirection_Right)
                            attack_bar_y = screen_y + 3;
                        else
                            attack_bar_y = screen_y - 1;
                    }
                    DrawHealthBar(attack_bar_x - 18, attack_bar_y - 25, player);
                }
                if (player->damage_shown)
                {
                    int attack_damage_x = screen_x;
                    int attack_damage_y = screen_y;
                    if (MainForm->weapon_mapper->IsRangedWeapon(player->weapon))
                    {
                        if (player->attack_frame == 1 || player->attack_frame == 2)
                        {
                            if (player->attack_direction <= FacingDirection_Left)
                                attack_damage_x = screen_x - 2;
                            else
                                attack_damage_x = screen_x + 2;
                            if (player->attack_direction == FacingDirection_Down ||
                                player->attack_direction == FacingDirection_Right)
                                attack_damage_y = screen_y + 1;
                        }
                    }
                    else if (player->attack_frame == 1 || player->attack_frame == 2 || player->attack_frame == 3)
                    {
                        if (player->attack_direction <= FacingDirection_Left)
                            attack_damage_x = screen_x - 6;
                        else
                            attack_damage_x = screen_x + 6;
                        if (player->attack_direction == FacingDirection_Down ||
                            player->attack_direction == FacingDirection_Right)
                            attack_damage_y = screen_y + 3;
                        else
                            attack_damage_y = screen_y - 1;
                    }
                    String attack_damage_text = IntToStr(player->damage_amount);
                    // No damage on a hit, rather than a heal, shows the miss label instead of digits.
                    if (player->damage_amount <= 0 && !player->damage_heal)
                        MainForm->gfx->game->BlitClipped(attack_damage_x - 13,
                            2 * player->damage_timer + attack_damage_y - 52 - 2 * player->gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            MainForm->gfx->interface_graphics->GetMissRect(player->damage_heal));
                    else
                    {
                        for (int attack_digit_index = 1; player->damage_digits >= attack_digit_index;
                            attack_digit_index++)
                        {
                            int attack_digit = StrToInt(attack_damage_text[attack_digit_index]);
                            MainForm->gfx->game->BlitClipped(attack_damage_x - 5 - 8 * player->damage_digits / 2 +
                                8 * attack_digit_index,
                                2 * player->damage_timer + attack_damage_y - 52 - 2 * player->gender,
                                MainForm->gfx->interface_graphics->hud_sheet,
                                MainForm->gfx->interface_graphics->GetDamageDigitRect(player->damage_heal,
                                    attack_digit));
                        }
                    }
                }
            }
            if (player->casting && idle)
            {
                idle = false;
                if (player->cast_frame <= 2)
                {
                    player_rect.left = screen_x - 34;
                    player_rect.right = screen_x + 34;
                    player_rect.top = screen_y - 18;
                    player_rect.bottom = screen_y + 66;
                    if (player->cast_direction > FacingDirection_Left)
                    {
                        player_rect.left += 18;
                        player_rect.right += 18;
                    }
                    if (player->cast_direction == FacingDirection_Down ||
                        player->cast_direction == FacingDirection_Right)
                        BlitFrame(player_rect, player->resources->front_sheet, player->resources->GetFrameRect(7), true,
                            true, player);
                    else
                        BlitFrame(player_rect, player->resources->back_sheet, player->resources->GetFrameRect(7), true,
                            true, player);
                    if (draw_buffered)
                        DrawTranslucentBuffer(player_rect);
                }
                else
                    draw_standing = true;
                if (player->health_bar_shown)
                    DrawHealthBar(screen_x - 18, screen_y - 25, player);
                if (player->damage_shown)
                {
                    String cast_damage_text = IntToStr(player->damage_amount);
                    for (int cast_digit_index = 1; player->damage_digits >= cast_digit_index; cast_digit_index++)
                    {
                        int cast_digit = StrToInt(cast_damage_text[cast_digit_index]);
                        if (player->damage_amount <= 0 && !player->damage_heal)
                            MainForm->gfx->game->BlitClipped(screen_x - 13,
                                2 * player->damage_timer + screen_y - 52 - 2 * player->gender,
                                MainForm->gfx->interface_graphics->hud_sheet,
                                MainForm->gfx->interface_graphics->GetMissRect(player->damage_heal));
                        else
                            MainForm->gfx->game->BlitClipped(screen_x - 5 - 8 * player->damage_digits / 2 +
                                8 * cast_digit_index, 2 * player->damage_timer + screen_y - 52 - 2 * player->gender,
                                MainForm->gfx->interface_graphics->hud_sheet,
                                MainForm->gfx->interface_graphics->GetDamageDigitRect(player->damage_heal, cast_digit));
                    }
                }
            }
            if (player->jumping && idle)
            {
                idle = false;
                if (player->jump_direction == FacingDirection_Left)
                {
                    player_rect.left = screen_x - 2 - (8 * player->jump_frame + 8);
                    player_rect.top = screen_y - 3 - (4 * player->jump_frame + 4);
                    player_rect.right = screen_x + 66 - (8 * player->jump_frame + 8);
                    player_rect.bottom = screen_y + 81 - (4 * player->jump_frame + 4);
                }
                if (player->jump_direction == FacingDirection_Up)
                {
                    player_rect.left = 8 * player->jump_frame + screen_x - 58;
                    player_rect.top = screen_y - 3 - (4 * player->jump_frame + 4);
                    player_rect.right = 8 * player->jump_frame + screen_x + 10;
                    player_rect.bottom = screen_y + 81 - (4 * player->jump_frame + 4);
                }
                if (player->jump_direction == FacingDirection_Right)
                {
                    player_rect.left = 8 * player->jump_frame + screen_x - 58;
                    player_rect.top = 4 * player->jump_frame + screen_y - 31;
                    player_rect.right = 8 * player->jump_frame + screen_x + 10;
                    player_rect.bottom = 4 * player->jump_frame + screen_y + 53;
                }
                if (player->jump_direction == FacingDirection_Down)
                {
                    player_rect.left = screen_x - 2 - (8 * player->jump_frame + 8);
                    player_rect.top = 4 * player->jump_frame + screen_y - 31;
                    player_rect.right = screen_x + 66 - (8 * player->jump_frame + 8);
                    player_rect.bottom = 4 * player->jump_frame + screen_y + 53;
                }
                // The jump lifts the body most in its middle step.
                if (player->jump_frame == 0 || player->jump_frame == 2)
                {
                    player_rect.top = player_rect.top - 14;
                    player_rect.bottom = player_rect.bottom - 14;
                }
                if (player->jump_frame == 1)
                {
                    player_rect.top = player_rect.top - 22;
                    player_rect.bottom = player_rect.bottom - 22;
                }
                if (player->direction > FacingDirection_Left)
                {
                    player_rect.left += 18;
                    player_rect.right += 18;
                }
                if (player->direction == FacingDirection_Down || player->direction == FacingDirection_Right)
                    BlitFrame(player_rect, player->resources->front_sheet, player->resources->GetFrameRect(1), true,
                        true, player);
                else
                    BlitFrame(player_rect, player->resources->back_sheet, player->resources->GetFrameRect(1), true,
                        true, player);
                if (player->health_bar_shown)
                {
                    int jump_bar_y;
                    int jump_bar_x;
                    if (player->jump_direction == FacingDirection_Left)
                    {
                        jump_bar_x = screen_x + 32 - (8 * player->jump_frame + 8);
                        jump_bar_y = screen_y + 16 - (4 * player->jump_frame + 4);
                    }
                    if (player->jump_direction == FacingDirection_Up)
                    {
                        jump_bar_x = 8 * player->jump_frame + screen_x - 24;
                        jump_bar_y = screen_y + 16 - (4 * player->jump_frame + 4);
                    }
                    if (player->jump_direction == FacingDirection_Right)
                    {
                        jump_bar_x = 8 * player->jump_frame + screen_x - 24;
                        jump_bar_y = 4 * player->jump_frame + screen_y - 12;
                    }
                    if (player->jump_direction == FacingDirection_Down)
                    {
                        jump_bar_x = screen_x + 32 - (8 * player->jump_frame + 8);
                        jump_bar_y = 4 * player->jump_frame + screen_y - 12;
                    }
                    if (player->jump_frame == 0 || player->jump_frame == 2)
                    {
                        jump_bar_y -= 14;
                        jump_bar_y -= 14;
                    }
                    if (player->jump_frame == 1)
                    {
                        jump_bar_y -= 22;
                        jump_bar_y -= 22;
                    }
                    DrawHealthBar(jump_bar_x - 18, jump_bar_y - 25, player);
                }
                if (player->damage_shown)
                {
                    int jump_damage_y;
                    int jump_damage_x;
                    if (player->jump_direction == FacingDirection_Left)
                    {
                        jump_damage_x = screen_x + 32 - (8 * player->jump_frame + 8);
                        jump_damage_y = screen_y + 16 - (4 * player->jump_frame + 4);
                    }
                    if (player->jump_direction == FacingDirection_Up)
                    {
                        jump_damage_x = 8 * player->jump_frame + screen_x - 24;
                        jump_damage_y = screen_y + 16 - (4 * player->jump_frame + 4);
                    }
                    if (player->jump_direction == FacingDirection_Right)
                    {
                        jump_damage_x = 8 * player->jump_frame + screen_x - 24;
                        jump_damage_y = 4 * player->jump_frame + screen_y - 12;
                    }
                    if (player->jump_direction == FacingDirection_Down)
                    {
                        jump_damage_x = screen_x + 32 - (8 * player->jump_frame + 8);
                        jump_damage_y = 4 * player->jump_frame + screen_y - 12;
                    }
                    if (player->jump_frame == 0 || player->jump_frame == 2)
                    {
                        jump_damage_y -= 14;
                        jump_damage_y -= 14;
                    }
                    if (player->jump_frame == 1)
                    {
                        jump_damage_y -= 22;
                        jump_damage_y -= 22;
                    }
                    String jump_damage_text = IntToStr(player->damage_amount);
                    if (player->damage_amount <= 0 && !player->damage_heal)
                        MainForm->gfx->game->BlitClipped(jump_damage_x - 13,
                            2 * player->damage_timer + jump_damage_y - 52 - 2 * player->gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            MainForm->gfx->interface_graphics->GetMissRect(player->damage_heal));
                    else
                    {
                        for (int jump_digit_index = 1; player->damage_digits >= jump_digit_index; jump_digit_index++)
                        {
                            int jump_digit = StrToInt(jump_damage_text[jump_digit_index]);
                            MainForm->gfx->game->BlitClipped(jump_damage_x - 5 - 8 * player->damage_digits / 2 +
                                8 * jump_digit_index,
                                2 * player->damage_timer + jump_damage_y - 52 - 2 * player->gender,
                                MainForm->gfx->interface_graphics->hud_sheet,
                                MainForm->gfx->interface_graphics->GetDamageDigitRect(player->damage_heal, jump_digit));
                        }
                    }
                }
                if (draw_buffered)
                    DrawTranslucentBuffer(player_rect);
            }
            if (player->walking && idle)
            {
                idle = false;
                if (player->walk_direction == FacingDirection_Left)
                {
                    player_rect.left = screen_x - 2 - (8 * player->walk_frame + 8);
                    player_rect.top = screen_y - 3 - (4 * player->walk_frame + 4);
                    player_rect.right = screen_x + 66 - (8 * player->walk_frame + 8);
                    player_rect.bottom = screen_y + 81 - (4 * player->walk_frame + 4);
                }
                if (player->walk_direction == FacingDirection_Up)
                {
                    player_rect.left = 8 * player->walk_frame + screen_x - 58;
                    player_rect.top = screen_y - 3 - (4 * player->walk_frame + 4);
                    player_rect.right = 8 * player->walk_frame + screen_x + 10;
                    player_rect.bottom = screen_y + 81 - (4 * player->walk_frame + 4);
                }
                if (player->walk_direction == FacingDirection_Right)
                {
                    player_rect.left = 8 * player->walk_frame + screen_x - 58;
                    player_rect.top = 4 * player->walk_frame + screen_y - 31;
                    player_rect.right = 8 * player->walk_frame + screen_x + 10;
                    player_rect.bottom = 4 * player->walk_frame + screen_y + 53;
                }
                if (player->walk_direction == FacingDirection_Down)
                {
                    player_rect.left = screen_x - 2 - (8 * player->walk_frame + 8);
                    player_rect.top = 4 * player->walk_frame + screen_y - 31;
                    player_rect.right = screen_x + 66 - (8 * player->walk_frame + 8);
                    player_rect.bottom = 4 * player->walk_frame + screen_y + 53;
                }
                if (player->direction > FacingDirection_Left)
                {
                    player_rect.left += 18;
                    player_rect.right += 18;
                }
                if (player->direction == FacingDirection_Down || player->direction == FacingDirection_Right)
                    BlitFrame(player_rect, player->resources->front_sheet,
                        player->resources->GetFrameRect(player->walk_frame + 1), true, true, player);
                else
                    BlitFrame(player_rect, player->resources->back_sheet,
                        player->resources->GetFrameRect(player->walk_frame + 1), true, true, player);
                if (player->health_bar_shown)
                {
                    int walk_bar_y;
                    int walk_bar_x;
                    if (player->walk_direction == FacingDirection_Left)
                    {
                        walk_bar_x = screen_x + 32 - (8 * player->walk_frame + 8);
                        walk_bar_y = screen_y + 16 - (4 * player->walk_frame + 4);
                    }
                    if (player->walk_direction == FacingDirection_Up)
                    {
                        walk_bar_x = 8 * player->walk_frame + screen_x - 24;
                        walk_bar_y = screen_y + 16 - (4 * player->walk_frame + 4);
                    }
                    if (player->walk_direction == FacingDirection_Right)
                    {
                        walk_bar_x = 8 * player->walk_frame + screen_x - 24;
                        walk_bar_y = 4 * player->walk_frame + screen_y - 12;
                    }
                    if (player->walk_direction == FacingDirection_Down)
                    {
                        walk_bar_x = screen_x + 32 - (8 * player->walk_frame + 8);
                        walk_bar_y = 4 * player->walk_frame + screen_y - 12;
                    }
                    DrawHealthBar(walk_bar_x - 18, walk_bar_y - 25, player);
                }
                if (player->damage_shown)
                {
                    int walk_damage_y;
                    int walk_damage_x;
                    if (player->walk_direction == FacingDirection_Left)
                    {
                        walk_damage_x = screen_x + 32 - (8 * player->walk_frame + 8);
                        walk_damage_y = screen_y + 16 - (4 * player->walk_frame + 4);
                    }
                    if (player->walk_direction == FacingDirection_Up)
                    {
                        walk_damage_x = 8 * player->walk_frame + screen_x - 24;
                        walk_damage_y = screen_y + 16 - (4 * player->walk_frame + 4);
                    }
                    if (player->walk_direction == FacingDirection_Right)
                    {
                        walk_damage_x = 8 * player->walk_frame + screen_x - 24;
                        walk_damage_y = 4 * player->walk_frame + screen_y - 12;
                    }
                    if (player->walk_direction == FacingDirection_Down)
                    {
                        walk_damage_x = screen_x + 32 - (8 * player->walk_frame + 8);
                        walk_damage_y = 4 * player->walk_frame + screen_y - 12;
                    }
                    String walk_damage_text = IntToStr(player->damage_amount);
                    if (player->damage_amount <= 0 && !player->damage_heal)
                        MainForm->gfx->game->BlitClipped(walk_damage_x - 13,
                            2 * player->damage_timer + walk_damage_y - 52 - 2 * player->gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            MainForm->gfx->interface_graphics->GetMissRect(player->damage_heal));
                    else
                    {
                        for (int walk_digit_index = 1; player->damage_digits >= walk_digit_index; walk_digit_index++)
                        {
                            int walk_digit = StrToInt(walk_damage_text[walk_digit_index]);
                            MainForm->gfx->game->BlitClipped(walk_damage_x - 5 - 8 * player->damage_digits / 2 +
                                8 * walk_digit_index,
                                2 * player->damage_timer + walk_damage_y - 52 - 2 * player->gender,
                                MainForm->gfx->interface_graphics->hud_sheet,
                                MainForm->gfx->interface_graphics->GetDamageDigitRect(player->damage_heal, walk_digit));
                        }
                    }
                }
                if (draw_buffered)
                    DrawTranslucentBuffer(player_rect);
            }
            if (player->sitting_on_floor && idle)
            {
                idle = false;
                player_rect.left = screen_x - 34;
                player_rect.right = screen_x + 34;
                if (player->direction > FacingDirection_Left)
                {
                    player_rect.left += 18;
                    player_rect.right += 18;
                }
                if (player->direction == FacingDirection_Down || player->direction == FacingDirection_Right)
                {
                    player_rect.top = screen_y - 5;
                    player_rect.bottom = screen_y + 79;
                    BlitFrame(player_rect, player->resources->front_sheet, player->resources->GetFrameRect(9), true,
                        true, player);
                }
                else
                {
                    player_rect.top = screen_y - 2;
                    player_rect.bottom = screen_y + 82;
                    BlitFrame(player_rect, player->resources->back_sheet, player->resources->GetFrameRect(9), true,
                        true, player);
                }
                // The emote face is drawn only for the two directions that use the front sheet.
                if (player->emoting)
                {
                    emote_pass = true;
                    emote_lowered = true;
                    direction = player->direction;
                    draw_gender = player->gender;
                    if (player->direction == FacingDirection_Down || player->direction == FacingDirection_Right)
                    {
                        emote_rect.left = player->direction / 3 + screen_x + 2;
                        emote_rect.right = player->direction / 3 + screen_x + 15;
                        emote_rect.top = screen_y + 17 - player->gender + 5;
                        emote_rect.bottom = screen_y + 31 - player->gender + 5;
                        BlitFrame(emote_rect, player->resources->emote_strip,
                            player->resources->GetEmoteFaceRect(player->emote_id - 1), true, true, player);
                    }
                }
                if (player->health_bar_shown)
                    DrawHealthBar(screen_x - 18, screen_y - 7, player);
                if (player->damage_shown)
                {
                    String floor_damage_text = IntToStr(player->damage_amount);
                    if (player->damage_amount <= 0 && !player->damage_heal)
                        MainForm->gfx->game->BlitClipped(screen_x - 13,
                            2 * player->damage_timer + screen_y - 35 - 2 * player->gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            MainForm->gfx->interface_graphics->GetMissRect(player->damage_heal));
                    else
                    {
                        for (int floor_digit_index = 1; player->damage_digits >= floor_digit_index; floor_digit_index++)
                        {
                            int floor_digit = StrToInt(floor_damage_text[floor_digit_index]);
                            MainForm->gfx->game->BlitClipped(screen_x - 5 - 8 * player->damage_digits / 2 +
                                8 * floor_digit_index, 2 * player->damage_timer + screen_y - 35 - 2 * player->gender,
                                MainForm->gfx->interface_graphics->hud_sheet,
                                MainForm->gfx->interface_graphics->GetDamageDigitRect(player->damage_heal,
                                    floor_digit));
                        }
                    }
                }
                if (draw_buffered)
                    DrawTranslucentBuffer(player_rect);
            }
            if (player->sitting_on_chair && idle)
            {
                idle = false;
                player_rect.left = screen_x - 34;
                player_rect.right = screen_x + 34;
                if (player->direction > FacingDirection_Left)
                {
                    player_rect.left += 18;
                    player_rect.right += 18;
                }
                if (player->direction == FacingDirection_Down || player->direction == FacingDirection_Right)
                {
                    player_rect.top = screen_y - 10;
                    player_rect.bottom = screen_y + 74;
                    BlitFrame(player_rect, player->resources->front_sheet, player->resources->GetFrameRect(8), true,
                        true, player);
                }
                else
                {
                    player_rect.top = screen_y - 8;
                    player_rect.bottom = screen_y + 76;
                    BlitFrame(player_rect, player->resources->back_sheet, player->resources->GetFrameRect(8), true,
                        true, player);
                }
                if (player->emoting)
                {
                    emote_pass = true;
                    emote_lowered = true;
                    direction = player->direction;
                    draw_gender = player->gender;
                    if (player->direction == FacingDirection_Down || player->direction == FacingDirection_Right)
                    {
                        emote_rect.left = player->direction / 3 + screen_x + 2;
                        emote_rect.right = player->direction / 3 + screen_x + 15;
                        emote_rect.top = screen_y + 17 - player->gender;
                        emote_rect.bottom = screen_y + 31 - player->gender;
                        BlitFrame(emote_rect, player->resources->emote_strip,
                            player->resources->GetEmoteFaceRect(player->emote_id - 1), true, true, player);
                    }
                }
                if (player->health_bar_shown)
                    DrawHealthBar(screen_x - 18, screen_y - 12, player);
                if (player->damage_shown)
                {
                    String chair_damage_text = IntToStr(player->damage_amount);
                    if (player->damage_amount <= 0 && !player->damage_heal)
                        MainForm->gfx->game->BlitClipped(screen_x - 13,
                            2 * player->damage_timer + screen_y - 39 - 2 * player->gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            MainForm->gfx->interface_graphics->GetMissRect(player->damage_heal));
                    else
                    {
                        for (int chair_digit_index = 1; player->damage_digits >= chair_digit_index; chair_digit_index++)
                        {
                            int chair_digit = StrToInt(chair_damage_text[chair_digit_index]);
                            MainForm->gfx->game->BlitClipped(screen_x - 5 - 8 * player->damage_digits / 2 +
                                8 * chair_digit_index, 2 * player->damage_timer + screen_y - 39 - 2 * player->gender,
                                MainForm->gfx->interface_graphics->hud_sheet,
                                MainForm->gfx->interface_graphics->GetDamageDigitRect(player->damage_heal,
                                    chair_digit));
                        }
                    }
                }
                if (draw_buffered)
                    DrawTranslucentBuffer(player_rect);
            }
            if (idle || draw_standing)
            {
                player_rect.left = screen_x - 34;
                player_rect.right = screen_x + 34;
                player_rect.top = screen_y - 18;
                player_rect.bottom = screen_y + 66;
                if (player->direction > FacingDirection_Left)
                {
                    player_rect.left += 18;
                    player_rect.right += 18;
                }
                if (player->direction == FacingDirection_Down || player->direction == FacingDirection_Right)
                    BlitFrame(player_rect, player->resources->front_sheet, player->resources->GetFrameRect(0), true,
                        true, player);
                else
                    BlitFrame(player_rect, player->resources->back_sheet, player->resources->GetFrameRect(0), true,
                        true, player);
                if (player->emoting)
                {
                    emote_pass = true;
                    direction = player->direction;
                    draw_gender = player->gender;
                    if (player->direction == FacingDirection_Down || player->direction == FacingDirection_Right)
                    {
                        emote_rect.left = player->direction / 3 + screen_x + 2;
                        emote_rect.right = player->direction / 3 + screen_x + 15;
                        emote_rect.top = screen_y + 3 - player->gender;
                        emote_rect.bottom = screen_y + 17 - player->gender;
                        BlitFrame(emote_rect, player->resources->emote_strip,
                            player->resources->GetEmoteFaceRect(player->emote_id - 1), true, true, player);
                    }
                }
                if (player->health_bar_shown)
                    DrawHealthBar(screen_x - 18, screen_y - 25, player);
                if (player->damage_shown)
                {
                    String stand_damage_text = IntToStr(player->damage_amount);
                    if (player->damage_amount <= 0 && !player->damage_heal)
                        MainForm->gfx->game->BlitClipped(screen_x - 13,
                            2 * player->damage_timer + screen_y - 52 - 2 * player->gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            MainForm->gfx->interface_graphics->GetMissRect(player->damage_heal));
                    else
                    {
                        for (int stand_digit_index = 1; player->damage_digits >= stand_digit_index; stand_digit_index++)
                        {
                            int stand_digit = StrToInt(stand_damage_text[stand_digit_index]);
                            MainForm->gfx->game->BlitClipped(screen_x - 5 - 8 * player->damage_digits / 2 +
                                8 * stand_digit_index, 2 * player->damage_timer + screen_y - 52 - 2 * player->gender,
                                MainForm->gfx->interface_graphics->hud_sheet,
                                MainForm->gfx->interface_graphics->GetDamageDigitRect(player->damage_heal,
                                    stand_digit));
                        }
                    }
                }
                if (draw_buffered)
                    DrawTranslucentBuffer(player_rect);
            }
        }
    }
    catch (...)
    {
    }
}

// Draws the standing frame of every player shown in the preview slot, as for one player at a point.
void SpriteGlobalGraphics::DrawPreviewPlayers(int group)
{
    int player_x;
    if (MainForm->players->GetCount() >= 1)
    {
        for (int i = 0; i < (int)MainForm->players->GetCount(); i++)
        {
            Player handle = MainForm->players->GetPlayer(i);
            if (handle.draw_group == group)
            {
                player_x = handle.x;
                int preview_y = handle.y;
                if (handle.direction >= FacingDirection_Up)
                {
                    blt_fx->FX = 2;
                    is_mirrored = true;
                }
                else
                {
                    blt_fx->FX = 0;
                    is_mirrored = false;
                }
                player_rect.left = player_x - 34;
                player_rect.right = player_x + 34;
                player_rect.top = preview_y - 18;
                player_rect.bottom = preview_y + 66;
                if (handle.direction > FacingDirection_Left)
                {
                    player_rect.left += 18;
                    player_rect.right += 18;
                }
                if (handle.direction == FacingDirection_Down || handle.direction == FacingDirection_Right)
                    BlitFrame(player_rect, handle.resources->front_sheet, handle.resources->GetFrameRect(0), false,
                        true, &handle);
                else
                    BlitFrame(player_rect, handle.resources->back_sheet, handle.resources->GetFrameRect(0), false, true,
                        &handle);
            }
        }
    }
}

void SpriteGlobalGraphics::BlitFrame(TRect dest_rect, TDx_DrawSurface *sheet, TRect source_rect, bool clip,
    bool translucent, Player *player)
{
    if (!clip)
    {
        if (translucent)
            MainForm->OffscreenSurface->Blt(&dest_rect, sheet, &source_rect, DDBLT_DDFX | DDBLT_KEYSRC | DDBLT_WAIT,
                blt_fx);
        else
            MainForm->OffscreenSurface->Blt(&dest_rect, sheet, &source_rect, DDBLT_DDFX | DDBLT_WAIT, blt_fx);
    }
    else
    {
        if (!emote_pass)
        {
            if (player->direction > FacingDirection_Left)
                MainForm->blitter->AddLight(dest_rect.left + 17, dest_rect.bottom - 14, 100, 100);
            else
                MainForm->blitter->AddLight(dest_rect.left + 34, dest_rect.bottom - 14, 100, 100);
        }
        char visible = true;
        int width = dest_rect.right - dest_rect.left;
        int height = dest_rect.bottom - dest_rect.top;
        if (dest_rect.left + width < view_left)
            visible = false;
        if (dest_rect.top + height < view_top)
            visible = false;
        if (dest_rect.left > view_right)
            visible = false;
        if (dest_rect.top > view_bottom)
            visible = false;
        if (visible)
        {
            dest_rect = ComputeClip(dest_rect);
            if (use_frame_buffer)
            {
                if (draw_progress > 1)
                {
                    TRect buffer_rect;
                    char unused_bytes[16];
                    if (!emote_pass)
                    {
                        buffer_rect.left = 0;
                        buffer_rect.top = 0;
                        buffer_rect.right = 68;
                        buffer_rect.bottom = 84;
                        translucent_buffer->Blt(&buffer_rect, sheet, &source_rect, DDBLT_DDFX | DDBLT_WAIT, blt_fx);
                    }
                    else
                    {
                        buffer_rect.left = direction / 3 + 36;
                        buffer_rect.top = 21 - draw_gender;
                        buffer_rect.right = direction / 3 + 49;
                        buffer_rect.bottom = 35 - draw_gender;
                        if (direction > FacingDirection_Left)
                        {
                            buffer_rect.left = buffer_rect.left - 18;
                            buffer_rect.right = buffer_rect.right - 18;
                        }
                        if (emote_lowered)
                        {
                            buffer_rect.top += 6;
                            buffer_rect.bottom += 6;
                        }
                        translucent_buffer->Blt(&buffer_rect, sheet, &source_rect, DDBLT_DDFX | DDBLT_WAIT, blt_fx);
                    }
                }
            }
            else if (translucent)
                MainForm->OffscreenSurface->Blt(&dest_rect, sheet, &ApplyClip(source_rect),
                    DDBLT_DDFX | DDBLT_KEYSRC | DDBLT_WAIT, blt_fx);
            else
                MainForm->OffscreenSurface->Blt(&dest_rect, sheet, &ApplyClip(source_rect), DDBLT_DDFX | DDBLT_WAIT,
                    blt_fx);
        }
    }
}

// Draws the buffered half-see-through sprite at the rectangle, clipped to the game view; nothing when it is off view.
void SpriteGlobalGraphics::DrawTranslucentBuffer(TRect dest_rect)
{
    int width;
    int cut_left;
    int cut_top;
    char visible = true;
    width = dest_rect.right - dest_rect.left;
    int height = dest_rect.bottom - dest_rect.top;
    if (width + dest_rect.left < view_left)
        visible = false;
    if (dest_rect.top + height < view_top)
        visible = false;
    if (dest_rect.left > view_right)
        visible = false;
    if (dest_rect.top > view_bottom)
        visible = false;
    if (visible)
    {
        TRect clip = ComputeClip(dest_rect);
        TRect buffer_rect;
        buffer_rect.left = 0;
        buffer_rect.top = 0;
        buffer_rect.right = 68;
        buffer_rect.bottom = 84;
        if (dest_rect.top < 8 || dest_rect.left < 9)
        {
            cut_left = 68 - (clip.right - clip.left);
            cut_top = 84 - (clip.bottom - clip.top);
            buffer_rect.left = cut_left;
            buffer_rect.top = cut_top;
            buffer_rect.right = 68;
            buffer_rect.bottom = 84;
        }
        MainForm->gfx->game->BlitClippedAlpha(clip.left - 9, clip.top - 8, translucent_buffer, buffer_rect, 100);
    }
}

// Draws what shows over the map's characters and items: the players' emotes, balloons and name or text labels, the
// npcs' balloons and labels, and the label of a hovered item.
void SpriteGlobalGraphics::DrawNameLabel()
{
    if (MainForm->players->GetCount() >= 1)
    {
        for (int i = 0; i < (int)MainForm->players->GetCount(); i++)
        {
            Player handle = MainForm->players->GetPlayer(i);
            if (!handle.health_bar_start && !handle.health_bar_shown && !handle.damage_start && !handle.damage_shown)
            {
                if (handle.emoting)
                {
                    // A hidden player's emote shows only to that player.
                    if ((!handle.hidden || MainForm->net_client->GetPlayerId() == handle.player_id) &&
                        !handle.walking && !handle.walk_start && !handle.jumping && !handle.jump_start)
                        handle.DrawEmote();
                }
                else if (handle.balloon_shown)
                    handle.DrawBalloon();
                else if (!handle.hidden || MainForm->net_client->GetPlayerId() == handle.player_id ||
                    MainForm->net_client->GetAdminLevel() > HIDDEN_VIEW_ADMIN_LEVEL)
                {
                    if (handle.hover_label_shown)
                        handle.DrawHoverLabel();
                    else if (handle.text_label_shown)
                    {
                        if (handle.text_label_blinking)
                        {
                            // A blinking label shows on half of the ticks.
                            if (handle.text_label_timer % 4 < 2)
                                handle.DrawTextLabel();
                        }
                        else
                            handle.DrawTextLabel();
                    }
                }
            }
        }
    }
    if (MainForm->npcs->GetCount() >= 1)
    {
        for (int j = 0; j < (int)MainForm->npcs->GetCount(); j++)
        {
            Npc npc = MainForm->npcs->GetNpc(j);
            // An npc with a health bar or damage number showing gets no label or balloon.
            if (!npc.health_bar_start && !npc.health_bar_shown && !npc.damage_start && !npc.damage_shown)
            {
                if (npc.balloon_shown)
                    npc.DrawBalloon();
                else if (npc.label_shown && !npc.walking && !npc.attacking && !npc.alt_attacking)
                {
                    int npc_label_x =
                        32 * npc.x - 32 * npc.y + MainForm->map->GetScrollOffsetX() + 306 -
                            (MainForm->gfx->hover_label->text_width - 4) / 2;
                    int npc_label_y =
                        16 * npc.x + 16 * npc.y + MainForm->map->GetScrollOffsetY() + 135 -
                            MainForm->gfx->hover_label->text_height;
                    npc_label_x += MainForm->animation->interface_controller->shake_offset;
                    // The npc's head height lifts the label above that graphic.
                    npc_label_y -= MainForm->npcs->head_heights[npc.graphic - 1];
                    MainForm->gfx->game->BlitClipped(npc_label_x, npc_label_y, MainForm->gfx->hover_label->surface,
                        MainForm->gfx->hover_label->surface_rect);
                }
            }
        }
    }
    if (MainForm->map_items->GetCount() >= 1 && MainForm->map_items->hover_label_shown)
    {
        for (int index = 0; index < (int)MainForm->map_items->GetCount(); index++)
        {
            MapItem map_item = MainForm->map_items->GetItem(index);
            if (map_item.label_shown)
            {
                int label_x =
                    32 * map_item.x - 32 * map_item.y + MainForm->map->GetScrollOffsetX() + 306 -
                        (MainForm->gfx->hover_label->text_width - 4) / 2;
                int label_y =
                    16 * map_item.x + 16 * map_item.y + MainForm->map->GetScrollOffsetY() + 135 -
                        MainForm->gfx->hover_label->text_height;
                label_x += MainForm->animation->interface_controller->shake_offset;
                MainForm->gfx->game->BlitClipped(label_x, label_y, MainForm->gfx->hover_label->surface,
                    MainForm->gfx->hover_label->surface_rect);
            }
        }
    }
}

// Cuts the destination rectangle to the view, keeping how much was cut from each side for the source.
TRect SpriteGlobalGraphics::ComputeClip(TRect dest_rect)
{
    TRect rect;
    rect = dest_rect;
    int width = dest_rect.right - dest_rect.left;
    int height = dest_rect.bottom - dest_rect.top;
    clip_left = 0;
    clip_top = 0;
    clip_right = 0;
    clip_bottom = 0;
    if (dest_rect.left < view_left)
    {
        clip_left = view_left - dest_rect.left;
        rect.left = view_left;
    }
    if (dest_rect.top < view_top)
    {
        clip_top = view_top - dest_rect.top;
        rect.top = view_top;
    }
    if (dest_rect.left + width > view_right)
    {
        clip_right = dest_rect.left + width - view_right;
        rect.right = view_right;
    }
    if (dest_rect.top + height > view_bottom)
    {
        clip_bottom = dest_rect.top + height - view_bottom;
        rect.bottom = view_bottom;
    }
    return rect;
}

// Cuts the same amounts from the source rectangle, the sides swapped for a mirrored frame.
TRect SpriteGlobalGraphics::ApplyClip(TRect source_rect)
{
    TRect rect;
    rect = source_rect;
    if (!is_mirrored)
    {
        if (clip_left > 0)
            rect.left += clip_left;
        if (clip_right > 0)
            rect.right -= clip_right;
    }
    else
    {
        if (clip_right > 0)
            rect.left += clip_right;
        if (clip_left > 0)
            rect.right -= clip_left;
    }
    if (clip_top > 0)
        rect.top += clip_top;
    if (clip_bottom > 0)
        rect.bottom -= clip_bottom;
    return rect;
}

// Shows the hover label with the text, sized to its width, in the colour and with the element icon when the setting
// shows them.
void SpriteGlobalGraphics::ShowNameLabel(String text, int color_index, int element)
{
    int width = 0;
    int height = 0;
    if (!MainForm->config->GetElementalIcons())
        element = 0;
    if (MainForm->fonts->BeginOffscreenIf(240))
    {
        try
        {
            MainForm->fonts->face = 1;
            MainForm->fonts->size = 8;
            width = (short)MainForm->fonts->TextWidth(text);
            height = 20;
        }
        catch (Exception &)
        {
        }
        MainForm->fonts->End();
    }
    MainForm->gfx->CreateHoverLabel(width, height, color_index, element, text);
}

// Creates the emote buffer, freeing an earlier one first.
void SpriteGlobalGraphics::CreateEmoteBuffer()
{
    if (emote_buffer_created)
        FreeEmoteBuffer();
    emote_buffer = new EmoteBuffer();
    emote_buffer_created = true;
}

void SpriteGlobalGraphics::FreeEmoteBuffer()
{
    if (emote_buffer_created)
    {
        delete emote_buffer;
        emote_buffer_created = false;
    }
}

// Draws a player's health bar over the point, its fill by the health left, in the bar kind's colour.
bool SpriteGlobalGraphics::DrawHealthBar(int x, int y, Player *player)
{
    MainForm->gfx->game->BlitClippedHalfBlend(x, y - 2 * player->gender, MainForm->gfx->interface_graphics->hud_sheet,
        MainForm->gfx->interface_graphics->health_bar_shadow_rect);
    MainForm->gfx->game->BlitClipped(x, y - 2 * player->gender, MainForm->gfx->interface_graphics->hud_sheet,
        MainForm->gfx->interface_graphics->health_bar_frame_rect);
    if (!player->health_bar_kind)
    {
        if (player->health_bar_width <= 5)
        {
            MainForm->gfx->game->BlitClipped(x + 2, y + 2 - 2 * player->gender,
                MainForm->gfx->interface_graphics->hud_sheet,
                MainForm->gfx->interface_graphics->GetHealthBarFillRect(HealthBarFill_Red, player->health_bar_width));
            return true;
        }
        if (player->health_bar_width > 5 && player->health_bar_width <= 12)
        {
            MainForm->gfx->game->BlitClipped(x + 2, y + 2 - 2 * player->gender,
                MainForm->gfx->interface_graphics->hud_sheet,
                MainForm->gfx->interface_graphics->GetHealthBarFillRect(HealthBarFill_Yellow,
                player->health_bar_width));
            return true;
        }
        MainForm->gfx->game->BlitClipped(x + 2, y + 2 - 2 * player->gender,
            MainForm->gfx->interface_graphics->hud_sheet,
            MainForm->gfx->interface_graphics->GetHealthBarFillRect(HealthBarFill_Green, player->health_bar_width));
        return true;
    }
    else if (player->health_bar_kind == 1)
    {
        MainForm->gfx->game->BlitClipped(x + 2, y + 2 - 2 * player->gender,
            MainForm->gfx->interface_graphics->hud_sheet,
            MainForm->gfx->interface_graphics->GetHealthBarFillRect(HealthBarFill_Second, player->second_bar_width));
        return true;
    }
    else if (player->health_bar_kind == 2)
    {
        MainForm->gfx->game->BlitClipped(x + 2, y + 2 - 2 * player->gender,
            MainForm->gfx->interface_graphics->hud_sheet,
            MainForm->gfx->interface_graphics->GetHealthBarFillRect(HealthBarFill_Third, player->third_bar_width));
        return true;
    }
    else
        return false;
}
