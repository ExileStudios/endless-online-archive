#include <vcl.h>
#pragma hdrstop

#include "Gfx_sprites.h"
#include "Tmainform.h"
#include "Gfx_emotebuffer.h"
#include "Gfx_interface.h"
#include "Gfx_game.h"
#include "Ctrl_interface.h"
#include "Gfx_playername.h"
#include "Gfx_player.h"
#pragma package(smart_init)

#define PLAYER_BAR_LOW_LENGTH 5
#define PLAYER_BAR_MID_LENGTH 12
#define SIT_DIRECTION_PITCH 48
#define SIT_FRAME_WIDTH 24
#define STAND_DIRECTION_PITCH 36
#define STAND_FRAME_WIDTH 18

// Creates each body sheet and the translucent frame buffer at its fixed size, and sets the view bounds that player
// frames are clipped to.
SpriteGraphics::SpriteGraphics()
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
    surface_desc->Flags = 6;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = 2112;
    surface_desc->Width = 72;
    surface_desc->Height = 406;
    stand_sheet->GetSurfaceDesc(surface_desc);
    stand_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 416;
    surface_desc->Height = 427;
    walk_sheet->GetSurfaceDesc(surface_desc);
    walk_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 192;
    surface_desc->Height = 406;
    attack_sheet->GetSurfaceDesc(surface_desc);
    attack_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 72;
    surface_desc->Height = 434;
    spell_cast_sheet->GetSurfaceDesc(surface_desc);
    spell_cast_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 96;
    surface_desc->Height = 364;
    chair_sit_sheet->GetSurfaceDesc(surface_desc);
    chair_sit_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 96;
    surface_desc->Height = 301;
    floor_sit_sheet->GetSurfaceDesc(surface_desc);
    floor_sit_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 100;
    surface_desc->Height = 406;
    ranged_attack_sheet->GetSurfaceDesc(surface_desc);
    ranged_attack_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 143;
    surface_desc->Height = 196;
    face_sheet->GetSurfaceDesc(surface_desc);
    face_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 68;
    surface_desc->Height = 84;
    translucent_buffer->GetSurfaceDesc(surface_desc);
    translucent_buffer->Create(surface_desc, MainForm->Dx_Draw1);
    emote_buffer_created = false;
}

SpriteGraphics::~SpriteGraphics()
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
    delete stand_sheet;
    delete walk_sheet;
    delete attack_sheet;
    delete spell_cast_sheet;
    delete chair_sit_sheet;
    delete floor_sit_sheet;
    delete ranged_attack_sheet;
    delete face_sheet;
    delete translucent_buffer;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    blt_fx->Clear();
    delete blt_fx;
}

// The rect of one standing body frame: gender picks the column block, direction the column, skin the row.
Windows::TRect Gfx_sprites_GetStandFrameRect(SpriteGraphics *self, int gender, int direction, int skin)
{
    TRect stand_frame_rect;
    // Directions 2 and 3 reuse the frames of 1 and 0; the draw mirrors them.
    if (direction >= FacingDirection_Up)
    {
        if (direction == FacingDirection_Up)
            direction = FacingDirection_Left;
        else
            direction = FacingDirection_Down;
    }
    stand_frame_rect.left = STAND_DIRECTION_PITCH * gender + STAND_FRAME_WIDTH * direction;
    stand_frame_rect.top = 58 * skin;
    stand_frame_rect.right = 36 * gender + 18 * direction + 18;
    stand_frame_rect.bottom = 58 * skin + 58;
    return stand_frame_rect;
}

// The rect of one walking body frame by gender, direction, skin and walk step, the step held to the sheet's four.
Windows::TRect Gfx_sprites_GetWalkFrameRect(SpriteGraphics *self, int gender, int direction, int skin, int frame)
{
    TRect walk_frame_rect;
    if (direction >= FacingDirection_Up)
    {
        if (direction == FacingDirection_Up)
            direction = FacingDirection_Left;
        else
            direction = FacingDirection_Down;
    }
    if (frame < 0)
        frame = 0;
    if (frame > 3)
        frame = 3;
    walk_frame_rect.left = 26 * frame + 208 * gender + 104 * direction;
    walk_frame_rect.top = 61 * skin;
    walk_frame_rect.right = 26 * frame + 208 * gender + 104 * direction + 26;
    walk_frame_rect.bottom = 61 * skin + 61;
    return walk_frame_rect;
}

// The rect of one melee attack body frame by gender, direction, skin and frame, the frame held to the sheet's two.
Windows::TRect Gfx_sprites_GetAttackFrameRect(SpriteGraphics *self, int gender, int direction, int skin, int frame)
{
    TRect attack_frame_rect;
    if (direction >= FacingDirection_Up)
    {
        if (direction == FacingDirection_Up)
            direction = FacingDirection_Left;
        else
            direction = FacingDirection_Down;
    }
    if (frame < 0)
        frame = 0;
    if (frame > 1)
        frame = 1;
    attack_frame_rect.left = 24 * frame + 96 * gender + 48 * direction;
    attack_frame_rect.top = 58 * skin;
    attack_frame_rect.right = 24 * frame + 96 * gender + 48 * direction + 24;
    attack_frame_rect.bottom = 58 * skin + 58;
    return attack_frame_rect;
}

// The rect of the spell cast body frame by gender, direction and skin.
Windows::TRect Gfx_sprites_GetSpellCastCellRect(SpriteGraphics *self, int gender, int direction, int skin)
{
    TRect sheet104_frame_rect;
    if (direction >= FacingDirection_Up)
    {
        if (direction == FacingDirection_Up)
            direction = FacingDirection_Left;
        else
            direction = FacingDirection_Down;
    }
    sheet104_frame_rect.left = STAND_DIRECTION_PITCH * gender + STAND_FRAME_WIDTH * direction;
    sheet104_frame_rect.top = 62 * skin;
    sheet104_frame_rect.right = 36 * gender + 18 * direction + 18;
    sheet104_frame_rect.bottom = 62 * skin + 62;
    return sheet104_frame_rect;
}

// The rect of the chair sitting body frame by gender, direction and skin.
Windows::TRect Gfx_sprites_GetChairSitFrameRect(SpriteGraphics *self, int gender, int direction, int skin)
{
    TRect chair_sit_frame_rect;
    if (direction >= FacingDirection_Up)
    {
        if (direction == FacingDirection_Up)
            direction = FacingDirection_Left;
        else
            direction = FacingDirection_Down;
    }
    chair_sit_frame_rect.left = SIT_DIRECTION_PITCH * gender + SIT_FRAME_WIDTH * direction;
    chair_sit_frame_rect.top = 52 * skin;
    chair_sit_frame_rect.right = 48 * gender + 24 * direction + 24;
    chair_sit_frame_rect.bottom = 52 * skin + 52;
    return chair_sit_frame_rect;
}

// The rect of the floor sitting body frame by gender, direction and skin.
Windows::TRect Gfx_sprites_GetFloorSitFrameRect(SpriteGraphics *self, int gender, int direction, int skin)
{
    TRect floor_sit_frame_rect;
    if (direction >= FacingDirection_Up)
    {
        if (direction == FacingDirection_Up)
            direction = FacingDirection_Left;
        else
            direction = FacingDirection_Down;
    }
    floor_sit_frame_rect.left = SIT_DIRECTION_PITCH * gender + SIT_FRAME_WIDTH * direction;
    floor_sit_frame_rect.top = 43 * skin;
    floor_sit_frame_rect.right = 48 * gender + 24 * direction + 24;
    floor_sit_frame_rect.bottom = 43 * skin + 43;
    return floor_sit_frame_rect;
}

// The rect of the ranged attack body frame by gender, direction and skin.
Windows::TRect Gfx_sprites_GetRangedAttackFrameRect(SpriteGraphics *self, int gender, int direction, int skin)
{
    TRect ranged_attack_frame_rect;
    if (direction >= FacingDirection_Up)
    {
        if (direction == FacingDirection_Up)
            direction = FacingDirection_Left;
        else
            direction = FacingDirection_Down;
    }
    ranged_attack_frame_rect.left = 50 * gender + 25 * direction;
    ranged_attack_frame_rect.top = 58 * skin;
    ranged_attack_frame_rect.right = 50 * gender + 25 * direction + 25;
    ranged_attack_frame_rect.bottom = 58 * skin + 58;
    return ranged_attack_frame_rect;
}

// The rect of the face strip for one gender and skin: one row holding every emote face side by side.
Windows::TRect Gfx_sprites_GetFaceStripRect(SpriteGraphics *self, int gender, int skin)
{
    TRect face_strip_rect;
    face_strip_rect.left = 0;
    face_strip_rect.right = 143;
    face_strip_rect.top = 98 * gender + 14 * skin;
    face_strip_rect.bottom = 98 * gender + 14 * skin + 14;
    return face_strip_rect;
}

// Draws the standing body bitmap onto its sheet, with black see-through.
void SpriteGraphics::LoadStandSheet(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (stand_sheet->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        stand_sheet->ReleaseDC(dc);
    }
    stand_sheet->SetColorKey(8, color_key);
}

// Draws the walking body bitmap onto its sheet, with black see-through.
void SpriteGraphics::LoadWalkSheet(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (walk_sheet->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        walk_sheet->ReleaseDC(dc);
    }
    walk_sheet->SetColorKey(8, color_key);
}

// Draws the melee attack body bitmap onto its sheet, with black see-through.
void SpriteGraphics::LoadAttackSheet(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (attack_sheet->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        attack_sheet->ReleaseDC(dc);
    }
    attack_sheet->SetColorKey(8, color_key);
}

// Draws the spell cast body bitmap onto its sheet, with black see-through.
void SpriteGraphics::LoadSpellCastSheet(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (spell_cast_sheet->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        spell_cast_sheet->ReleaseDC(dc);
    }
    spell_cast_sheet->SetColorKey(8, color_key);
}

// Draws the chair sitting body bitmap onto its sheet, with black see-through.
void SpriteGraphics::LoadChairSitSheet(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (chair_sit_sheet->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        chair_sit_sheet->ReleaseDC(dc);
    }
    chair_sit_sheet->SetColorKey(8, color_key);
}

// Draws the floor sitting body bitmap onto its sheet, with black see-through.
void SpriteGraphics::LoadFloorSitSheet(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (floor_sit_sheet->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        floor_sit_sheet->ReleaseDC(dc);
    }
    floor_sit_sheet->SetColorKey(8, color_key);
}

// Draws the ranged attack body bitmap onto its sheet, with black see-through.
void SpriteGraphics::LoadRangedAttackSheet(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (ranged_attack_sheet->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        ranged_attack_sheet->ReleaseDC(dc);
    }
    ranged_attack_sheet->SetColorKey(8, color_key);
}

// Draws the emote face bitmap onto its sheet, with black see-through.
void SpriteGraphics::LoadFaceSheet(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (face_sheet->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        face_sheet->ReleaseDC(dc);
    }
    face_sheet->SetColorKey(8, color_key);
}

// Draws a player's standing frame unclipped at a screen position, facing the player's direction.
void SpriteGraphics::DrawPlayerAt(Player player, int hair_style, int hair_color, int x, int y)
{
    int screen_x = 0;
    int screen_y = 0;
    screen_x = x;
    screen_y = y;
    if (player.direction >= FacingDirection_Up)
    {
        blt_fx->FX = 2;
        is_mirrored = 1;
    }
    else
    {
        blt_fx->FX = 0;
        is_mirrored = 0;
    }
    frame_rect.left = screen_x - 34;
    frame_rect.right = screen_x + 34;
    frame_rect.top = screen_y - 18;
    frame_rect.bottom = screen_y + 66;
    if (player.direction > FacingDirection_Left)
    {
        frame_rect.left += 18;
        frame_rect.right += 18;
    }
    if ((player.direction == FacingDirection_Down) || player.direction == FacingDirection_Right)
        BlitFrame(frame_rect, player.resources->front_sheet, player.resources->Gfx_player_GetFrameRect(0), false, true);
    else
        BlitFrame(frame_rect, player.resources->back_sheet, player.resources->Gfx_player_GetFrameRect(0), false, true);
}

// Draws one player on the map in the first state that holds: attacking, casting, jumping, walking, sitting on the
// floor, sitting on a chair, else standing. Each state also draws the overhead health bar and damage number while
// they show, and the standing and sitting states draw the emote face. With draw_buffered set the frame goes to the
// translucent buffer, which is then blended over the map.
void SpriteGraphics::DrawPlayer(Player player, int draw_buffered, int x_offset)
{
    if (player.graphics_pending)
        player_load_stage = player.load_progress;
    else
        player_load_stage = 99;
    player_load_stage = 99;
    use_frame_buffer = draw_buffered;
    emote_pass = false;
    emote_lowered = false;
    try
    {
        // Players of a preview group other than 0 are not drawn on the map.
        if (!player.slot_state)
        {
            int screen_x = TILE_HALF_WIDTH * player.x - TILE_HALF_WIDTH * player.y + MainForm->map->GetScrollOffsetX() + 310;
            int screen_y = TILE_HALF_HEIGHT * player.x + TILE_HALF_HEIGHT * player.y + MainForm->map->GetScrollOffsetY() + 102;
            if (x_offset)
                screen_x += x_offset;
            // Directions 2 and 3 are the frames of 1 and 0 drawn mirrored.
            if (player.direction >= FacingDirection_Up)
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
            if (player.attacking && no_state_drawn)
            {
                no_state_drawn = 0;
                // A ranged attack shows its frame on the middle two steps; the other steps draw standing.
                if (MainForm->weapon_mapper->IsRangedWeapon(player.weapon))
                {
                    if (player.attack_frame == 1 || player.attack_frame == 2)
                    {
                        if (player.attack_direction <= FacingDirection_Left)
                        {
                            frame_rect.left = screen_x - 40;
                            frame_rect.right = screen_x + 28;
                        }
                        else
                        {
                            frame_rect.left = screen_x - 28;
                            frame_rect.right = screen_x + 40;
                        }
                        if ((player.attack_direction == FacingDirection_Down) || player.attack_direction == FacingDirection_Right)
                        {
                            frame_rect.top = screen_y - 17;
                            frame_rect.bottom = screen_y + 67;
                        }
                        else
                        {
                            frame_rect.top = screen_y - 19;
                            frame_rect.bottom = screen_y + 65;
                        }
                        if (player.attack_direction == FacingDirection_Left || player.attack_direction == FacingDirection_Up)
                        {
                            frame_rect.top++;
                            frame_rect.bottom++;
                            if (player.attack_direction == FacingDirection_Left)
                            {
                                frame_rect.left += 2;
                                frame_rect.right += 2;
                            }
                            if (player.attack_direction == FacingDirection_Up)
                            {
                                frame_rect.left = frame_rect.left - 2;
                                frame_rect.right = frame_rect.right - 2;
                            }
                        }
                        if (player.attack_direction > FacingDirection_Left)
                        {
                            frame_rect.left += 18;
                            frame_rect.right += 18;
                        }
                        if ((player.attack_direction == FacingDirection_Down) || player.attack_direction == FacingDirection_Right)
                            BlitFrame(frame_rect, player.resources->front_sheet,
                                player.resources->Gfx_player_GetFrameRect(10), true, true);
                        else
                            BlitFrame(frame_rect, player.resources->back_sheet,
                                player.resources->Gfx_player_GetFrameRect(10), true, true);
                        if ((char)draw_buffered)
                            DrawTranslucentBuffer(frame_rect);
                    }
                    else
                        state_expired = 1;
                }
                else if (player.attack_frame <= 3)
                {
                    // A melee attack shows its first frame on step 0 and its second frame from step 1 on.
                    int attack_frame = player.attack_frame + 5;
                    if (attack_frame >= 6)
                    {
                        attack_frame = 6;
                        if (player.attack_direction <= FacingDirection_Left)
                        {
                            frame_rect.left = screen_x - 40;
                            frame_rect.right = screen_x + 28;
                        }
                        else
                        {
                            frame_rect.left = screen_x - 28;
                            frame_rect.right = screen_x + 40;
                        }
                        if ((player.attack_direction == FacingDirection_Down) || player.attack_direction == FacingDirection_Right)
                        {
                            frame_rect.top = screen_y - 17;
                            frame_rect.bottom = screen_y + 67;
                        }
                        else
                        {
                            frame_rect.top = screen_y - 19;
                            frame_rect.bottom = screen_y + 65;
                        }
                    }
                    else
                    {
                        frame_rect.left = screen_x - 34;
                        frame_rect.right = screen_x + 34;
                        frame_rect.top = screen_y - 18;
                        frame_rect.bottom = screen_y + 66;
                    }
                    if (player.attack_direction > FacingDirection_Left)
                    {
                        frame_rect.left += 18;
                        frame_rect.right += 18;
                    }
                    if ((player.attack_direction == FacingDirection_Down) || player.attack_direction == FacingDirection_Right)
                        BlitFrame(frame_rect, player.resources->front_sheet,
                            player.resources->Gfx_player_GetFrameRect(attack_frame), true, true);
                    else
                        BlitFrame(frame_rect, player.resources->back_sheet,
                            player.resources->Gfx_player_GetFrameRect(attack_frame), true, true);
                    if ((char)draw_buffered)
                        DrawTranslucentBuffer(frame_rect);
                }
                else
                    state_expired = 1;
                if (player.health_bar_shown)
                {
                    int attack_bar_x = screen_x;
                    int attack_bar_y = screen_y;
                    if (MainForm->weapon_mapper->IsRangedWeapon(player.weapon))
                    {
                        if (player.attack_frame == 1 || player.attack_frame == 2)
                        {
                            if (player.attack_direction <= FacingDirection_Left)
                                attack_bar_x = screen_x - 2;
                            else
                                attack_bar_x = screen_x + 2;
                            if ((player.attack_direction == FacingDirection_Down) || player.attack_direction == FacingDirection_Right)
                                attack_bar_y = screen_y + 1;
                        }
                    }
                    else if (player.attack_frame == 1 || player.attack_frame == 2 || player.attack_frame == 3)
                    {
                        if (player.attack_direction <= FacingDirection_Left)
                            attack_bar_x = screen_x - 6;
                        else
                            attack_bar_x = screen_x + 6;
                        if ((player.attack_direction == FacingDirection_Down) || player.attack_direction == FacingDirection_Right)
                            attack_bar_y = screen_y + 3;
                        else
                            attack_bar_y = screen_y - 1;
                    }
                    MainForm->gfx->game->BlitClippedHalfBlend(attack_bar_x - 18, attack_bar_y - 25 - 2 * player.gender,
                        MainForm->gfx->interface_graphics->hud_sheet,
                        MainForm->gfx->interface_graphics->health_bar_shadow_rect);
                    MainForm->gfx->game->BlitClipped(attack_bar_x - 18, attack_bar_y - 25 - 2 * player.gender,
                        MainForm->gfx->interface_graphics->hud_sheet,
                        MainForm->gfx->interface_graphics->health_bar_frame_rect);
                    if (player.health_bar_width <= PLAYER_BAR_LOW_LENGTH)
                        MainForm->gfx->game->BlitClipped(attack_bar_x - 16, attack_bar_y - 23 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 2,
                            player.health_bar_width));
                    if (player.health_bar_width > PLAYER_BAR_LOW_LENGTH && player.health_bar_width <= PLAYER_BAR_MID_LENGTH)
                        MainForm->gfx->game->BlitClipped(attack_bar_x - 16, attack_bar_y - 23 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 1,
                            player.health_bar_width));
                    if (player.health_bar_width > PLAYER_BAR_MID_LENGTH)
                        MainForm->gfx->game->BlitClipped(attack_bar_x - 16, attack_bar_y - 23 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 0,
                            player.health_bar_width));
                }
                if (player.damage_shown)
                {
                    int attack_damage_x = screen_x;
                    int attack_damage_y = screen_y;
                    if (MainForm->weapon_mapper->IsRangedWeapon(player.weapon))
                    {
                        if (player.attack_frame == 1 || player.attack_frame == 2)
                        {
                            if (player.attack_direction <= FacingDirection_Left)
                                attack_damage_x = screen_x - 2;
                            else
                                attack_damage_x = screen_x + 2;
                            if ((player.attack_direction == FacingDirection_Down) || player.attack_direction == FacingDirection_Right)
                                attack_damage_y = screen_y + 1;
                        }
                    }
                    else if (player.attack_frame == 1 || player.attack_frame == 2 || player.attack_frame == 3)
                    {
                        if (player.attack_direction <= FacingDirection_Left)
                            attack_damage_x = screen_x - 6;
                        else
                            attack_damage_x = screen_x + 6;
                        if ((player.attack_direction == FacingDirection_Down) || player.attack_direction == FacingDirection_Right)
                            attack_damage_y = screen_y + 3;
                        else
                            attack_damage_y = screen_y - 1;
                    }
                    String attack_damage_text = IntToStr(player.damage_amount);
                    // No damage on a hit, rather than a heal, shows the miss label instead of digits.
                    if (player.damage_amount <= 0 && !player.damage_heal)
                        MainForm->gfx->game->BlitClipped(attack_damage_x - 13,
                            2 * player.damage_timer + attack_damage_y - 52 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            MainForm->gfx->interface_graphics->miss_label_rect);
                    else
                    {
                        for (int i = 1; i <= player.damage_digits; i++)
                        {
                            int attack_digit = StrToInt(String(attack_damage_text[i]));
                            if (!player.damage_heal)
                                MainForm->gfx->game->BlitClipped(attack_damage_x - 5 - 8 * player.damage_digits / 2 +
                                    8 * i, 2 * player.damage_timer + attack_damage_y - 52 - 2 * player.gender,
                                    MainForm->gfx->interface_graphics->hud_sheet,
                                    Gfx_interface_GetDamageDigitRect((int)MainForm->gfx->interface_graphics, 0,
                                    attack_digit));
                            else
                                MainForm->gfx->game->BlitClipped(attack_damage_x - 5 - 8 * player.damage_digits / 2 +
                                    8 * i, 2 * player.damage_timer + attack_damage_y - 52 - 2 * player.gender,
                                    MainForm->gfx->interface_graphics->hud_sheet,
                                    Gfx_interface_GetDamageDigitRect((int)MainForm->gfx->interface_graphics, 1,
                                    attack_digit));
                        }
                    }
                }
            }
            if (player.casting && no_state_drawn)
            {
                no_state_drawn = 0;
                if (player.cast_frame <= 2)
                {
                    frame_rect.left = screen_x - 34;
                    frame_rect.right = screen_x + 34;
                    frame_rect.top = screen_y - 18;
                    frame_rect.bottom = screen_y + 66;
                    if (player.cast_direction > FacingDirection_Left)
                    {
                        frame_rect.left += 18;
                        frame_rect.right += 18;
                    }
                    if (player.cast_direction == FacingDirection_Down || player.cast_direction == FacingDirection_Right)
                        BlitFrame(frame_rect, player.resources->front_sheet,
                            player.resources->Gfx_player_GetFrameRect(7), true, true);
                    else
                        BlitFrame(frame_rect, player.resources->back_sheet,
                            player.resources->Gfx_player_GetFrameRect(7), true, true);
                    if ((char)draw_buffered)
                        DrawTranslucentBuffer(frame_rect);
                }
                else
                    state_expired = 1;
                if (player.health_bar_shown)
                {
                    MainForm->gfx->game->BlitClippedHalfBlend(screen_x - 18, screen_y - 25 - 2 * player.gender,
                        MainForm->gfx->interface_graphics->hud_sheet,
                        MainForm->gfx->interface_graphics->health_bar_shadow_rect);
                    MainForm->gfx->game->BlitClipped(screen_x - 18, screen_y - 25 - 2 * player.gender,
                        MainForm->gfx->interface_graphics->hud_sheet,
                        MainForm->gfx->interface_graphics->health_bar_frame_rect);
                    if (player.health_bar_width <= PLAYER_BAR_LOW_LENGTH)
                        MainForm->gfx->game->BlitClipped(screen_x - 16, screen_y - 23 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 2,
                            player.health_bar_width));
                    if (player.health_bar_width > PLAYER_BAR_LOW_LENGTH && player.health_bar_width <= PLAYER_BAR_MID_LENGTH)
                        MainForm->gfx->game->BlitClipped(screen_x - 16, screen_y - 23 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 1,
                            player.health_bar_width));
                    if (player.health_bar_width > PLAYER_BAR_MID_LENGTH)
                        MainForm->gfx->game->BlitClipped(screen_x - 16, screen_y - 23 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 0,
                            player.health_bar_width));
                }
                if (player.damage_shown)
                {
                    String pose_damage_text = IntToStr(player.damage_amount);
                    for (int j = 1; j <= player.damage_digits; j++)
                    {
                        int pose_digit = StrToInt(String(pose_damage_text[j]));
                        if (player.damage_amount <= 0 && !player.damage_heal)
                            MainForm->gfx->game->BlitClipped(screen_x - 13,
                                2 * player.damage_timer + screen_y - 52 - 2 * player.gender,
                                MainForm->gfx->interface_graphics->hud_sheet,
                                MainForm->gfx->interface_graphics->miss_label_rect);
                        else
                        {
                            if (!player.damage_heal)
                                MainForm->gfx->game->BlitClipped(screen_x - 5 - 8 * player.damage_digits / 2 + 8 * j,
                                    2 * player.damage_timer + screen_y - 52 - 2 * player.gender,
                                    MainForm->gfx->interface_graphics->hud_sheet,
                                    Gfx_interface_GetDamageDigitRect((int)MainForm->gfx->interface_graphics, 0,
                                    pose_digit));
                            else
                                MainForm->gfx->game->BlitClipped(screen_x - 5 - 8 * player.damage_digits / 2 + 8 * j,
                                    2 * player.damage_timer + screen_y - 52 - 2 * player.gender,
                                    MainForm->gfx->interface_graphics->hud_sheet,
                                    Gfx_interface_GetDamageDigitRect((int)MainForm->gfx->interface_graphics, 1,
                                    pose_digit));
                        }
                    }
                }
            }
            if (player.jumping && no_state_drawn)
            {
                no_state_drawn = 0;
                if (player.jump_direction == FacingDirection_Left)
                {
                    frame_rect.left = screen_x - 2 - (8 * player.jump_frame + 8);
                    frame_rect.top = screen_y - 3 - (4 * player.jump_frame + 4);
                    frame_rect.right = screen_x + 66 - (8 * player.jump_frame + 8);
                    frame_rect.bottom = screen_y + 81 - (4 * player.jump_frame + 4);
                }
                if (player.jump_direction == FacingDirection_Up)
                {
                    frame_rect.left = screen_x + 8 * player.jump_frame - 58;
                    frame_rect.top = screen_y - 3 - (4 * player.jump_frame + 4);
                    frame_rect.right = screen_x + 8 * player.jump_frame + 10;
                    frame_rect.bottom = screen_y + 81 - (4 * player.jump_frame + 4);
                }
                if (player.jump_direction == FacingDirection_Right)
                {
                    frame_rect.left = screen_x + 8 * player.jump_frame - 58;
                    frame_rect.top = 4 * player.jump_frame + screen_y - 31;
                    frame_rect.right = screen_x + 8 * player.jump_frame + 10;
                    frame_rect.bottom = 4 * player.jump_frame + screen_y + 53;
                }
                if (player.jump_direction == FacingDirection_Down)
                {
                    frame_rect.left = screen_x - 2 - (8 * player.jump_frame + 8);
                    frame_rect.top = 4 * player.jump_frame + screen_y - 31;
                    frame_rect.right = screen_x + 66 - (8 * player.jump_frame + 8);
                    frame_rect.bottom = 4 * player.jump_frame + screen_y + 53;
                }
                // The jump lifts the body most in its middle step.
                if (!player.jump_frame || player.jump_frame == 2)
                {
                    frame_rect.top = frame_rect.top - 14;
                    frame_rect.bottom = frame_rect.bottom - 14;
                }
                if (player.jump_frame == 1)
                {
                    frame_rect.top = frame_rect.top - 22;
                    frame_rect.bottom = frame_rect.bottom - 22;
                }
                if (player.direction > FacingDirection_Left)
                {
                    frame_rect.left += 18;
                    frame_rect.right += 18;
                }
                if ((player.direction == FacingDirection_Down) || player.direction == FacingDirection_Right)
                    BlitFrame(frame_rect, player.resources->front_sheet, player.resources->Gfx_player_GetFrameRect(1),
                        true, true);
                else
                    BlitFrame(frame_rect, player.resources->back_sheet, player.resources->Gfx_player_GetFrameRect(1),
                        true, true);
                if (player.health_bar_shown)
                {
                    int move_bar_y;
                    int move_bar_x;
                    if (player.jump_direction == FacingDirection_Left)
                    {
                        move_bar_x = screen_x + 32 - (8 * player.jump_frame + 8);
                        move_bar_y = screen_y + 16 - (4 * player.jump_frame + 4);
                    }
                    if (player.jump_direction == FacingDirection_Up)
                    {
                        move_bar_x = screen_x + 8 * player.jump_frame - 24;
                        move_bar_y = screen_y + 16 - (4 * player.jump_frame + 4);
                    }
                    if (player.jump_direction == FacingDirection_Right)
                    {
                        move_bar_x = screen_x + 8 * player.jump_frame - 24;
                        move_bar_y = 4 * player.jump_frame + screen_y - 12;
                    }
                    if (player.jump_direction == FacingDirection_Down)
                    {
                        move_bar_x = screen_x + 32 - (8 * player.jump_frame + 8);
                        move_bar_y = 4 * player.jump_frame + screen_y - 12;
                    }
                    if (!player.jump_frame || player.jump_frame == 2)
                    {
                        move_bar_y -= 14;
                        move_bar_y -= 14;
                    }
                    if (player.jump_frame == 1)
                    {
                        move_bar_y -= 22;
                        move_bar_y -= 22;
                    }
                    MainForm->gfx->game->BlitClippedHalfBlend(move_bar_x - 18, move_bar_y - 25 - 2 * player.gender,
                        MainForm->gfx->interface_graphics->hud_sheet,
                        MainForm->gfx->interface_graphics->health_bar_shadow_rect);
                    MainForm->gfx->game->BlitClipped(move_bar_x - 18, move_bar_y - 25 - 2 * player.gender,
                        MainForm->gfx->interface_graphics->hud_sheet,
                        MainForm->gfx->interface_graphics->health_bar_frame_rect);
                    if (player.health_bar_width <= PLAYER_BAR_LOW_LENGTH)
                        MainForm->gfx->game->BlitClipped(move_bar_x - 16, move_bar_y - 23 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 2,
                            player.health_bar_width));
                    if (player.health_bar_width > PLAYER_BAR_LOW_LENGTH && player.health_bar_width <= PLAYER_BAR_MID_LENGTH)
                        MainForm->gfx->game->BlitClipped(move_bar_x - 16, move_bar_y - 23 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 1,
                            player.health_bar_width));
                    if (player.health_bar_width > PLAYER_BAR_MID_LENGTH)
                        MainForm->gfx->game->BlitClipped(move_bar_x - 16, move_bar_y - 23 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 0,
                            player.health_bar_width));
                }
                if (player.damage_shown)
                {
                    int move_damage_y;
                    int move_damage_x;
                    if (player.jump_direction == FacingDirection_Left)
                    {
                        move_damage_x = screen_x + 32 - (8 * player.jump_frame + 8);
                        move_damage_y = screen_y + 16 - (4 * player.jump_frame + 4);
                    }
                    if (player.jump_direction == FacingDirection_Up)
                    {
                        move_damage_x = screen_x + 8 * player.jump_frame - 24;
                        move_damage_y = screen_y + 16 - (4 * player.jump_frame + 4);
                    }
                    if (player.jump_direction == FacingDirection_Right)
                    {
                        move_damage_x = screen_x + 8 * player.jump_frame - 24;
                        move_damage_y = 4 * player.jump_frame + screen_y - 12;
                    }
                    if (player.jump_direction == FacingDirection_Down)
                    {
                        move_damage_x = screen_x + 32 - (8 * player.jump_frame + 8);
                        move_damage_y = 4 * player.jump_frame + screen_y - 12;
                    }
                    if (!player.jump_frame || player.jump_frame == 2)
                    {
                        move_damage_y -= 14;
                        move_damage_y -= 14;
                    }
                    if (player.jump_frame == 1)
                    {
                        move_damage_y -= 22;
                        move_damage_y -= 22;
                    }
                    String move_damage_text = IntToStr(player.damage_amount);
                    if (player.damage_amount <= 0 && !player.damage_heal)
                        MainForm->gfx->game->BlitClipped(move_damage_x - 13,
                            2 * player.damage_timer + move_damage_y - 52 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            MainForm->gfx->interface_graphics->miss_label_rect);
                    else
                    {
                        for (int k = 1; k <= player.damage_digits; k++)
                        {
                            int move_digit = StrToInt(String(move_damage_text[k]));
                            if (!player.damage_heal)
                                MainForm->gfx->game->BlitClipped(move_damage_x - 5 - 8 * player.damage_digits / 2 +
                                    8 * k, 2 * player.damage_timer + move_damage_y - 52 - 2 * player.gender,
                                    MainForm->gfx->interface_graphics->hud_sheet,
                                    Gfx_interface_GetDamageDigitRect((int)MainForm->gfx->interface_graphics, 0,
                                    move_digit));
                            else
                                MainForm->gfx->game->BlitClipped(move_damage_x - 5 - 8 * player.damage_digits / 2 +
                                    8 * k, 2 * player.damage_timer + move_damage_y - 52 - 2 * player.gender,
                                    MainForm->gfx->interface_graphics->hud_sheet,
                                    Gfx_interface_GetDamageDigitRect((int)MainForm->gfx->interface_graphics, 1,
                                    move_digit));
                        }
                    }
                }
                if ((char)draw_buffered)
                    DrawTranslucentBuffer(frame_rect);
            }
            if (player.walking && no_state_drawn)
            {
                no_state_drawn = 0;
                if (player.walk_direction == FacingDirection_Left)
                {
                    frame_rect.left = screen_x - 2 - (8 * player.walk_frame + 8);
                    frame_rect.top = screen_y - 3 - (4 * player.walk_frame + 4);
                    frame_rect.right = screen_x + 66 - (8 * player.walk_frame + 8);
                    frame_rect.bottom = screen_y + 81 - (4 * player.walk_frame + 4);
                }
                if (player.walk_direction == FacingDirection_Up)
                {
                    frame_rect.left = screen_x + 8 * player.walk_frame - 58;
                    frame_rect.top = screen_y - 3 - (4 * player.walk_frame + 4);
                    frame_rect.right = screen_x + 8 * player.walk_frame + 10;
                    frame_rect.bottom = screen_y + 81 - (4 * player.walk_frame + 4);
                }
                if (player.walk_direction == FacingDirection_Right)
                {
                    frame_rect.left = screen_x + 8 * player.walk_frame - 58;
                    frame_rect.top = 4 * player.walk_frame + screen_y - 31;
                    frame_rect.right = screen_x + 8 * player.walk_frame + 10;
                    frame_rect.bottom = 4 * player.walk_frame + screen_y + 53;
                }
                if (player.walk_direction == FacingDirection_Down)
                {
                    frame_rect.left = screen_x - 2 - (8 * player.walk_frame + 8);
                    frame_rect.top = 4 * player.walk_frame + screen_y - 31;
                    frame_rect.right = screen_x + 66 - (8 * player.walk_frame + 8);
                    frame_rect.bottom = 4 * player.walk_frame + screen_y + 53;
                }
                if (player.direction > FacingDirection_Left)
                {
                    frame_rect.left += 18;
                    frame_rect.right += 18;
                }
                if ((player.direction == FacingDirection_Down) || player.direction == FacingDirection_Right)
                    BlitFrame(frame_rect, player.resources->front_sheet,
                        player.resources->Gfx_player_GetFrameRect(player.walk_frame + 1), true, true);
                else
                    BlitFrame(frame_rect, player.resources->back_sheet,
                        player.resources->Gfx_player_GetFrameRect(player.walk_frame + 1), true, true);
                if (player.health_bar_shown)
                {
                    int walk_bar_y;
                    int walk_bar_x;
                    if (player.walk_direction == FacingDirection_Left)
                    {
                        walk_bar_x = screen_x + 32 - (8 * player.walk_frame + 8);
                        walk_bar_y = screen_y + 16 - (4 * player.walk_frame + 4);
                    }
                    if (player.walk_direction == FacingDirection_Up)
                    {
                        walk_bar_x = screen_x + 8 * player.walk_frame - 24;
                        walk_bar_y = screen_y + 16 - (4 * player.walk_frame + 4);
                    }
                    if (player.walk_direction == FacingDirection_Right)
                    {
                        walk_bar_x = screen_x + 8 * player.walk_frame - 24;
                        walk_bar_y = 4 * player.walk_frame + screen_y - 12;
                    }
                    if (player.walk_direction == FacingDirection_Down)
                    {
                        walk_bar_x = screen_x + 32 - (8 * player.walk_frame + 8);
                        walk_bar_y = 4 * player.walk_frame + screen_y - 12;
                    }
                    MainForm->gfx->game->BlitClippedHalfBlend(walk_bar_x - 18, walk_bar_y - 25 - 2 * player.gender,
                        MainForm->gfx->interface_graphics->hud_sheet,
                        MainForm->gfx->interface_graphics->health_bar_shadow_rect);
                    MainForm->gfx->game->BlitClipped(walk_bar_x - 18, walk_bar_y - 25 - 2 * player.gender,
                        MainForm->gfx->interface_graphics->hud_sheet,
                        MainForm->gfx->interface_graphics->health_bar_frame_rect);
                    if (player.health_bar_width <= PLAYER_BAR_LOW_LENGTH)
                        MainForm->gfx->game->BlitClipped(walk_bar_x - 16, walk_bar_y - 23 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 2,
                            player.health_bar_width));
                    if (player.health_bar_width > PLAYER_BAR_LOW_LENGTH && player.health_bar_width <= PLAYER_BAR_MID_LENGTH)
                        MainForm->gfx->game->BlitClipped(walk_bar_x - 16, walk_bar_y - 23 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 1,
                            player.health_bar_width));
                    if (player.health_bar_width > PLAYER_BAR_MID_LENGTH)
                        MainForm->gfx->game->BlitClipped(walk_bar_x - 16, walk_bar_y - 23 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 0,
                            player.health_bar_width));
                }
                if (player.damage_shown)
                {
                    int walk_damage_y;
                    int walk_damage_x;
                    if (player.walk_direction == FacingDirection_Left)
                    {
                        walk_damage_x = screen_x + 32 - (8 * player.walk_frame + 8);
                        walk_damage_y = screen_y + 16 - (4 * player.walk_frame + 4);
                    }
                    if (player.walk_direction == FacingDirection_Up)
                    {
                        walk_damage_x = screen_x + 8 * player.walk_frame - 24;
                        walk_damage_y = screen_y + 16 - (4 * player.walk_frame + 4);
                    }
                    if (player.walk_direction == FacingDirection_Right)
                    {
                        walk_damage_x = screen_x + 8 * player.walk_frame - 24;
                        walk_damage_y = 4 * player.walk_frame + screen_y - 12;
                    }
                    if (player.walk_direction == FacingDirection_Down)
                    {
                        walk_damage_x = screen_x + 32 - (8 * player.walk_frame + 8);
                        walk_damage_y = 4 * player.walk_frame + screen_y - 12;
                    }
                    String walk_damage_text = IntToStr(player.damage_amount);
                    if (player.damage_amount <= 0 && !player.damage_heal)
                        MainForm->gfx->game->BlitClipped(walk_damage_x - 13,
                            2 * player.damage_timer + walk_damage_y - 52 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            MainForm->gfx->interface_graphics->miss_label_rect);
                    else
                    {
                        for (int m = 1; m <= player.damage_digits; m++)
                        {
                            int walk_digit = StrToInt(String(walk_damage_text[m]));
                            if (!player.damage_heal)
                                MainForm->gfx->game->BlitClipped(walk_damage_x - 5 - 8 * player.damage_digits / 2 +
                                    8 * m, 2 * player.damage_timer + walk_damage_y - 52 - 2 * player.gender,
                                    MainForm->gfx->interface_graphics->hud_sheet,
                                    Gfx_interface_GetDamageDigitRect((int)MainForm->gfx->interface_graphics, 0,
                                    walk_digit));
                            else
                                MainForm->gfx->game->BlitClipped(walk_damage_x - 5 - 8 * player.damage_digits / 2 +
                                    8 * m, 2 * player.damage_timer + walk_damage_y - 52 - 2 * player.gender,
                                    MainForm->gfx->interface_graphics->hud_sheet,
                                    Gfx_interface_GetDamageDigitRect((int)MainForm->gfx->interface_graphics, 1,
                                    walk_digit));
                        }
                    }
                }
                if ((char)draw_buffered)
                    DrawTranslucentBuffer(frame_rect);
            }
            if (player.sitting_on_floor && no_state_drawn)
            {
                no_state_drawn = 0;
                frame_rect.left = screen_x - 34;
                frame_rect.right = screen_x + 34;
                if (player.direction > FacingDirection_Left)
                {
                    frame_rect.left += 18;
                    frame_rect.right += 18;
                }
                if ((player.direction == FacingDirection_Down) || player.direction == FacingDirection_Right)
                {
                    frame_rect.top = screen_y - 5;
                    frame_rect.bottom = screen_y + 79;
                    BlitFrame(frame_rect, player.resources->front_sheet, player.resources->Gfx_player_GetFrameRect(9),
                        true, true);
                }
                else
                {
                    frame_rect.top = screen_y - 2;
                    frame_rect.bottom = screen_y + 82;
                    BlitFrame(frame_rect, player.resources->back_sheet, player.resources->Gfx_player_GetFrameRect(9),
                        true, true);
                }
                // The emote face is drawn only for the two directions that use the front sheet.
                if (player.emoting)
                {
                    emote_pass = true;
                    emote_lowered = true;
                    direction = player.direction;
                    height_offset = player.gender;
                    if ((player.direction == FacingDirection_Down) || player.direction == FacingDirection_Right)
                    {
                        emote_rect.left = screen_x + player.direction / 3 + 2;
                        emote_rect.right = screen_x + player.direction / 3 + 15;
                        emote_rect.top = screen_y + 17 - player.gender + 5;
                        emote_rect.bottom = screen_y + 31 - player.gender + 5;
                        BlitFrame(emote_rect, player.resources->emote_strip,
                            player.resources->Gfx_player_GetEmoteFaceRect(player.emote_id - 1), true, true);
                    }
                }
                if (player.health_bar_shown)
                {
                    MainForm->gfx->game->BlitClippedHalfBlend(screen_x - 18, screen_y - 7 - 2 * player.gender,
                        MainForm->gfx->interface_graphics->hud_sheet,
                        MainForm->gfx->interface_graphics->health_bar_shadow_rect);
                    MainForm->gfx->game->BlitClipped(screen_x - 18, screen_y - 7 - 2 * player.gender,
                        MainForm->gfx->interface_graphics->hud_sheet,
                        MainForm->gfx->interface_graphics->health_bar_frame_rect);
                    if (player.health_bar_width <= PLAYER_BAR_LOW_LENGTH)
                        MainForm->gfx->game->BlitClipped(screen_x - 16, screen_y - 5 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 2,
                            player.health_bar_width));
                    if (player.health_bar_width > PLAYER_BAR_LOW_LENGTH && player.health_bar_width <= PLAYER_BAR_MID_LENGTH)
                        MainForm->gfx->game->BlitClipped(screen_x - 16, screen_y - 5 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 1,
                            player.health_bar_width));
                    if (player.health_bar_width > PLAYER_BAR_MID_LENGTH)
                        MainForm->gfx->game->BlitClipped(screen_x - 16, screen_y - 5 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 0,
                            player.health_bar_width));
                }
                if (player.damage_shown)
                {
                    String floor_sit_damage_text = IntToStr(player.damage_amount);
                    if (player.damage_amount <= 0 && !player.damage_heal)
                        MainForm->gfx->game->BlitClipped(screen_x - 13,
                            2 * player.damage_timer + screen_y - 35 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            MainForm->gfx->interface_graphics->miss_label_rect);
                    else
                    {
                        for (int n = 1; n <= player.damage_digits; n++)
                        {
                            int floor_sit_digit = StrToInt(String(floor_sit_damage_text[n]));
                            if (!player.damage_heal)
                                MainForm->gfx->game->BlitClipped(screen_x - 5 - 8 * player.damage_digits / 2 + 8 * n,
                                    2 * player.damage_timer + screen_y - 35 - 2 * player.gender,
                                    MainForm->gfx->interface_graphics->hud_sheet,
                                    Gfx_interface_GetDamageDigitRect((int)MainForm->gfx->interface_graphics, 0,
                                    floor_sit_digit));
                            else
                                MainForm->gfx->game->BlitClipped(screen_x - 5 - 8 * player.damage_digits / 2 + 8 * n,
                                    2 * player.damage_timer + screen_y - 35 - 2 * player.gender,
                                    MainForm->gfx->interface_graphics->hud_sheet,
                                    Gfx_interface_GetDamageDigitRect((int)MainForm->gfx->interface_graphics, 1,
                                    floor_sit_digit));
                        }
                    }
                }
                if ((char)draw_buffered)
                    DrawTranslucentBuffer(frame_rect);
            }
            if (player.sitting_on_chair && no_state_drawn)
            {
                no_state_drawn = 0;
                frame_rect.left = screen_x - 34;
                frame_rect.right = screen_x + 34;
                if (player.direction > FacingDirection_Left)
                {
                    frame_rect.left += 18;
                    frame_rect.right += 18;
                }
                if ((player.direction == FacingDirection_Down) || player.direction == FacingDirection_Right)
                {
                    frame_rect.top = screen_y - 10;
                    frame_rect.bottom = screen_y + 74;
                    BlitFrame(frame_rect, player.resources->front_sheet, player.resources->Gfx_player_GetFrameRect(8),
                        true, true);
                }
                else
                {
                    frame_rect.top = screen_y - 8;
                    frame_rect.bottom = screen_y + 76;
                    BlitFrame(frame_rect, player.resources->back_sheet, player.resources->Gfx_player_GetFrameRect(8),
                        true, true);
                }
                if (player.emoting)
                {
                    emote_pass = true;
                    emote_lowered = true;
                    direction = player.direction;
                    height_offset = player.gender;
                    if ((player.direction == FacingDirection_Down) || player.direction == FacingDirection_Right)
                    {
                        emote_rect.left = screen_x + player.direction / 3 + 2;
                        emote_rect.right = screen_x + player.direction / 3 + 15;
                        emote_rect.top = screen_y + 17 - player.gender;
                        emote_rect.bottom = screen_y + 31 - player.gender;
                        BlitFrame(emote_rect, player.resources->emote_strip,
                            player.resources->Gfx_player_GetEmoteFaceRect(player.emote_id - 1), true, true);
                    }
                }
                if (player.health_bar_shown)
                {
                    MainForm->gfx->game->BlitClippedHalfBlend(screen_x - 18, screen_y - 12 - 2 * player.gender,
                        MainForm->gfx->interface_graphics->hud_sheet,
                        MainForm->gfx->interface_graphics->health_bar_shadow_rect);
                    MainForm->gfx->game->BlitClipped(screen_x - 18, screen_y - 12 - 2 * player.gender,
                        MainForm->gfx->interface_graphics->hud_sheet,
                        MainForm->gfx->interface_graphics->health_bar_frame_rect);
                    if (player.health_bar_width <= PLAYER_BAR_LOW_LENGTH)
                        MainForm->gfx->game->BlitClipped(screen_x - 16, screen_y - 10 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 2,
                            player.health_bar_width));
                    if (player.health_bar_width > PLAYER_BAR_LOW_LENGTH && player.health_bar_width <= PLAYER_BAR_MID_LENGTH)
                        MainForm->gfx->game->BlitClipped(screen_x - 16, screen_y - 10 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 1,
                            player.health_bar_width));
                    if (player.health_bar_width > PLAYER_BAR_MID_LENGTH)
                        MainForm->gfx->game->BlitClipped(screen_x - 16, screen_y - 10 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 0,
                            player.health_bar_width));
                }
                if (player.damage_shown)
                {
                    String chair_sit_damage_text = IntToStr(player.damage_amount);
                    if (player.damage_amount <= 0 && !player.damage_heal)
                        MainForm->gfx->game->BlitClipped(screen_x - 13,
                            2 * player.damage_timer + screen_y - 39 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            MainForm->gfx->interface_graphics->miss_label_rect);
                    else
                    {
                        for (int ii = 1; ii <= player.damage_digits; ii++)
                        {
                            int chair_sit_digit = StrToInt(String(chair_sit_damage_text[ii]));
                            if (!player.damage_heal)
                                MainForm->gfx->game->BlitClipped(screen_x - 5 - 8 * player.damage_digits / 2 + 8 * ii,
                                    2 * player.damage_timer + screen_y - 39 - 2 * player.gender,
                                    MainForm->gfx->interface_graphics->hud_sheet,
                                    Gfx_interface_GetDamageDigitRect((int)MainForm->gfx->interface_graphics, 0,
                                    chair_sit_digit));
                            else
                                MainForm->gfx->game->BlitClipped(screen_x - 5 - 8 * player.damage_digits / 2 + 8 * ii,
                                    2 * player.damage_timer + screen_y - 39 - 2 * player.gender,
                                    MainForm->gfx->interface_graphics->hud_sheet,
                                    Gfx_interface_GetDamageDigitRect((int)MainForm->gfx->interface_graphics, 1,
                                    chair_sit_digit));
                        }
                    }
                }
                if ((char)draw_buffered)
                    DrawTranslucentBuffer(frame_rect);
            }
            // A finished attack or cast step also falls back to the standing frame.
            if (no_state_drawn || state_expired)
            {
                frame_rect.left = screen_x - 34;
                frame_rect.right = screen_x + 34;
                frame_rect.top = screen_y - 18;
                frame_rect.bottom = screen_y + 66;
                if (player.direction > FacingDirection_Left)
                {
                    frame_rect.left += 18;
                    frame_rect.right += 18;
                }
                if ((player.direction == FacingDirection_Down) || player.direction == FacingDirection_Right)
                    BlitFrame(frame_rect, player.resources->front_sheet, player.resources->Gfx_player_GetFrameRect(0),
                        true, true);
                else
                    BlitFrame(frame_rect, player.resources->back_sheet, player.resources->Gfx_player_GetFrameRect(0),
                        true, true);
                if (player.emoting)
                {
                    emote_pass = true;
                    direction = player.direction;
                    height_offset = player.gender;
                    if ((player.direction == FacingDirection_Down) || player.direction == FacingDirection_Right)
                    {
                        emote_rect.left = screen_x + player.direction / 3 + 2;
                        emote_rect.right = screen_x + player.direction / 3 + 15;
                        emote_rect.top = screen_y + 3 - player.gender;
                        emote_rect.bottom = screen_y + 17 - player.gender;
                        BlitFrame(emote_rect, player.resources->emote_strip,
                            player.resources->Gfx_player_GetEmoteFaceRect(player.emote_id - 1), true, true);
                    }
                }
                if (player.health_bar_shown)
                {
                    MainForm->gfx->game->BlitClippedHalfBlend(screen_x - 18, screen_y - 25 - 2 * player.gender,
                        MainForm->gfx->interface_graphics->hud_sheet,
                        MainForm->gfx->interface_graphics->health_bar_shadow_rect);
                    MainForm->gfx->game->BlitClipped(screen_x - 18, screen_y - 25 - 2 * player.gender,
                        MainForm->gfx->interface_graphics->hud_sheet,
                        MainForm->gfx->interface_graphics->health_bar_frame_rect);
                    if (player.health_bar_width <= PLAYER_BAR_LOW_LENGTH)
                        MainForm->gfx->game->BlitClipped(screen_x - 16, screen_y - 23 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 2,
                            player.health_bar_width));
                    if (player.health_bar_width > PLAYER_BAR_LOW_LENGTH && player.health_bar_width <= PLAYER_BAR_MID_LENGTH)
                        MainForm->gfx->game->BlitClipped(screen_x - 16, screen_y - 23 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 1,
                            player.health_bar_width));
                    if (player.health_bar_width > PLAYER_BAR_MID_LENGTH)
                        MainForm->gfx->game->BlitClipped(screen_x - 16, screen_y - 23 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            Gfx_interface_GetHealthBarRect((int)MainForm->gfx->interface_graphics, 0,
                            player.health_bar_width));
                }
                if (player.damage_shown)
                {
                    String stand_damage_text = IntToStr(player.damage_amount);
                    if (player.damage_amount <= 0 && !player.damage_heal)
                        MainForm->gfx->game->BlitClipped(screen_x - 13,
                            2 * player.damage_timer + screen_y - 52 - 2 * player.gender,
                            MainForm->gfx->interface_graphics->hud_sheet,
                            MainForm->gfx->interface_graphics->miss_label_rect);
                    else
                    {
                        for (int jj = 1; jj <= player.damage_digits; jj++)
                        {
                            int stand_digit = StrToInt(String(stand_damage_text[jj]));
                            if (!player.damage_heal)
                                MainForm->gfx->game->BlitClipped(screen_x - 5 - 8 * player.damage_digits / 2 + 8 * jj,
                                    2 * player.damage_timer + screen_y - 52 - 2 * player.gender,
                                    MainForm->gfx->interface_graphics->hud_sheet,
                                    Gfx_interface_GetDamageDigitRect((int)MainForm->gfx->interface_graphics, 0,
                                    stand_digit));
                            else
                                MainForm->gfx->game->BlitClipped(screen_x - 5 - 8 * player.damage_digits / 2 + 8 * jj,
                                    2 * player.damage_timer + screen_y - 52 - 2 * player.gender,
                                    MainForm->gfx->interface_graphics->hud_sheet,
                                    Gfx_interface_GetDamageDigitRect((int)MainForm->gfx->interface_graphics, 1,
                                    stand_digit));
                        }
                    }
                }
                if ((char)draw_buffered)
                    DrawTranslucentBuffer(frame_rect);
            }
        }
    }
    catch (...)
    {
    }
}

// Draws, standing, each player of one preview group at its stored position read as screen pixels. The character
// select screen passes group 0 and the create character form passes group 1.
void SpriteGraphics::DrawPreviewPlayers(int preview)
{
    int i;
    int player_x;
    if (MainForm->players->GetCount() >= 1)
    {
        for (i = 0; i < (int)MainForm->players->GetCount(); i++)
        {
            Player handle = MainForm->players->GetPlayer(i);
            if (handle.slot_state == preview)
            {
                player_x = handle.x;
                int player_y = handle.y;
                if (handle.direction >= FacingDirection_Up)
                {
                    blt_fx->FX = 2;
                    is_mirrored = 1;
                }
                else
                {
                    blt_fx->FX = 0;
                    is_mirrored = 0;
                }
                frame_rect.left = player_x - 34;
                frame_rect.right = player_x + 34;
                frame_rect.top = player_y - 18;
                frame_rect.bottom = player_y + 66;
                if (handle.direction > FacingDirection_Left)
                {
                    frame_rect.left += 18;
                    frame_rect.right += 18;
                }
                if ((handle.direction == FacingDirection_Down) || handle.direction == FacingDirection_Right)
                    BlitFrame(frame_rect, handle.resources->front_sheet, handle.resources->Gfx_player_GetFrameRect(0),
                        false, true);
                else
                    BlitFrame(frame_rect, handle.resources->back_sheet, handle.resources->Gfx_player_GetFrameRect(0),
                        false, true);
            }
        }
    }
}

// Blits one frame of a player sheet. Clipped frames off the view are skipped. With the frame buffer in use the
// frame, or in the emote pass the face placed by direction and height, goes into the translucent buffer; otherwise
// the frame is clipped to the view and blitted to the screen, keyed or not, mirrored as the player faces.
void SpriteGraphics::BlitFrame(Windows::TRect dest_rect, TDx_DrawSurface *sheet, Windows::TRect source_rect, bool clip,
    bool keyed)
{
    int dest_width;
    int dest_height;
    if (!clip)
    {
        if (keyed)
            MainForm->OffscreenSurface->Blt(&dest_rect, sheet, &source_rect, 16812032, blt_fx);
        else
            MainForm->OffscreenSurface->Blt(&dest_rect, sheet, &source_rect, 16779264, blt_fx);
    }
    else
    {
        unsigned char result = 1;
        dest_width = dest_rect.right - dest_rect.left;
        dest_height = dest_rect.bottom - dest_rect.top;
        if (dest_rect.left + dest_width < view_left)
            result = 0;
        if (dest_rect.top + dest_height < view_top)
            result = 0;
        if (dest_rect.left > view_right)
            result = 0;
        if (dest_rect.top > view_bottom)
            result = 0;
        if (result)
        {
            TRect clipped_dest = ClipToView(this, dest_rect);
            dest_rect = clipped_dest;
            if (use_frame_buffer)
            {
                if (player_load_stage > 1)
                {
                    TRect face_left, v22;
                    if (!emote_pass)
                    {
                        face_left.left = 0;
                        face_left.top = 0;
                        face_left.right = 68;
                        face_left.bottom = 84;
                        translucent_buffer->Blt(&face_left, sheet, &source_rect, 16779264, blt_fx);
                    }
                    else
                    {
                        face_left.left = direction / 3 + 36;
                        face_left.top = 21 - height_offset;
                        face_left.right = direction / 3 + 49;
                        face_left.bottom = 35 - height_offset;
                        if (direction > FacingDirection_Left)
                        {
                            face_left.left = face_left.left - 18;
                            face_left.right = face_left.right - 18;
                        }
                        if (emote_lowered)
                        {
                            face_left.top += 6;
                            face_left.bottom += 6;
                        }
                        translucent_buffer->Blt(&face_left, sheet, &source_rect, 16779264, blt_fx);
                    }
                }
            }
            else
            {
                if (keyed)
                {
                    TRect keyed_clipped_source = GetClippedSourceRect(this, source_rect);
                    MainForm->OffscreenSurface->Blt(&dest_rect, sheet, &keyed_clipped_source, 16812032, blt_fx);
                }
                else
                {
                    TRect clipped_source = GetClippedSourceRect(this, source_rect);
                    MainForm->OffscreenSurface->Blt(&dest_rect, sheet, &clipped_source, 16779264, blt_fx);
                }
            }
        }
    }
}

// Blends the translucent frame buffer over the screen at a player's frame rect, clipped to the view.
void SpriteGraphics::DrawTranslucentBuffer(Windows::TRect dest_rect)
{
    int dest_width;
    int dest_height;
    char result = 1;
    dest_width = dest_rect.right - dest_rect.left;
    dest_height = dest_rect.bottom - dest_rect.top;
    if (dest_rect.left + dest_width < view_left)
        result = 0;
    if (dest_rect.top + dest_height < view_top)
        result = 0;
    if (dest_rect.left > view_right)
        result = 0;
    if (dest_rect.top > view_bottom)
        result = 0;
    if (result)
    {
        TRect clipped_left = ClipToView(this, dest_rect);
        Windows::TRect buffer_rect;
        buffer_rect.left = 0;
        buffer_rect.top = 0;
        buffer_rect.right = 68;
        buffer_rect.bottom = 84;
        // Past the view's top or left edge, the visible part is the buffer's bottom-right corner.
        if (dest_rect.top < 8 || dest_rect.left < 9)
        {
            int buffer_left = 68 - (clipped_left.right - clipped_left.left);
            int buffer_top = 84 - (clipped_left.bottom - clipped_left.top);
            buffer_rect.left = buffer_left;
            buffer_rect.top = buffer_top;
            buffer_rect.right = 68;
            buffer_rect.bottom = 84;
        }
        MainForm->gfx->game->BlitClippedAlpha(clipped_left.left - 9, clipped_left.top - 8, translucent_buffer,
            buffer_rect, 100);
        return;
    }
}

// Draws the labels above the map after the sprites: for each player an emote, a speech balloon or a name label; for
// each npc a balloon or its hover name; and the hover label of each map item under the mouse.
void SpriteGraphics::DrawNameLabel()
{
    int npc_label_x;
    int npc_label_y;
    int i;
    int j;
    int k;
    if (MainForm->players->GetCount() >= 1)
    {
        for (i = 0; i < (int)MainForm->players->GetCount(); i++)
        {
            Player player = MainForm->players->GetPlayer(i);
            // A player with a health bar or damage number showing gets no label or balloon.
            if (!player.health_bar_start && !player.health_bar_shown && !player.damage_start && !player.damage_shown)
            {
                if (player.emoting)
                {
                    // A hidden player's emote shows only to that player.
                    if (!player.hidden || MainForm->net_client->GetPlayerId() == player.player_id)
                    {
                        if (!player.walking && !player.walk_start && !player.jumping && !player.jump_start)
                            player.DrawEmote();
                    }
                }
                else if (player.balloon_shown)
                    player.DrawBalloon();
                else if (!player.hidden || MainForm->net_client->GetPlayerId() == player.player_id ||
                    // A hidden player's name label also shows to high-level admins.
                    MainForm->net_client->GetAdminLevel() > HIDDEN_VIEW_ADMIN_LEVEL)
                {
                    if (player.hover_label_shown)
                        player.DrawHoverLabel();
                    else if (player.text_label_shown)
                    {
                        if (player.text_label_blinking)
                        {
                            // A blinking label shows on half of the ticks.
                            if (player.text_label_timer % 4 < 2)
                                player.DrawTextLabel();
                        }
                        else
                            player.DrawTextLabel();
                    }
                }
            }
        }
    }
    if (MainForm->npcs->GetCount() >= 1)
    {
        for (j = 0; j < (int)MainForm->npcs->GetCount(); j++)
        {
            Npc npc = MainForm->npcs->GetNpc(j);
            if (!npc.health_bar_start && !npc.health_bar_shown && !npc.damage_start && !npc.damage_shown)
            {
                if (npc.balloon_shown)
                    npc.DrawBalloon();
                else if (npc.label_shown && !npc.walking && !npc.attacking && !npc.alt_attacking)
                {
                    npc_label_x = 32 * npc.x - 32 * npc.y + MainForm->map->GetScrollOffsetX() + 306 -
                        (MainForm->gfx->hover_label->text_width - 4) / 2;
                    npc_label_y = 16 * npc.x + 16 * npc.y + MainForm->map->GetScrollOffsetY() + 135 -
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
    if (MainForm->map_items->GetCount() >= 1)
    {
        if (MainForm->map_items->hover_label_shown)
        {
            for (k = 0; k < (int)MainForm->map_items->GetCount(); k++)
            {
                MapItem handle = MainForm->map_items->GetItem(k);
                if (handle.label_shown)
                {
                    int item_label_x = 32 * handle.x - 32 * handle.y + MainForm->map->GetScrollOffsetX() + 306 -
                        (MainForm->gfx->hover_label->text_width - 4) / 2;
                    int item_label_y = 16 * handle.x + 16 * handle.y + MainForm->map->GetScrollOffsetY() + 135 -
                        MainForm->gfx->hover_label->text_height;
                    item_label_x += MainForm->animation->interface_controller->shake_offset;
                    MainForm->gfx->game->BlitClipped(item_label_x, item_label_y, MainForm->gfx->hover_label->surface,
                        MainForm->gfx->hover_label->surface_rect);
                }
            }
        }
    }
}

// Clips a frame rect to the view and records how much was cut from each side, for GetClippedSourceRect.
DEFINE_CLIP_TO_VIEW(SpriteGraphics)

// Trims a sheet rect by the amounts ClipToView cut, swapping the left and right cuts for a mirrored frame.
DEFINE_GET_CLIPPED_SOURCE_RECT(SpriteGraphics)

// Measures a name label's text in the label font and builds the shared hover label surface for it.
void SpriteGraphics::ShowNameLabel(String text, int color_index)
{
    int text_width;
    unsigned int label_height;
    HDC dc;
    text_width = 0;
    label_height = 0;
    if (MainForm->OffscreenSurface->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Brush->Color = clBlack;
            MainForm->draw_canvas->Brush->Style = bsClear;
            MainForm->draw_canvas->Pen->Color = clWhite;
            MainForm->draw_canvas->Font->Color = clWhite;
            MainForm->draw_canvas->Font->Name = "MS Sans Serif";
            MainForm->draw_canvas->Font->Size = 8;
            text_width = MainForm->draw_canvas->TextWidth(text);
            label_height = 20;
            MainForm->draw_canvas->Handle = 0;
        }
        catch (Exception &)
        {
        }
        MainForm->OffscreenSurface->ReleaseDC(dc);
    }
    MainForm->gfx->CreateHoverLabel(text_width, label_height, color_index, text);
}

// Wraps a chat line into at most six balloon lines and opens the balloon over the player, when balloons are on.
void SpriteGraphics::ShowSpeechBalloon(String text, int player_id, int style)
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
                        // A line breaks at a space once it is fairly wide, or anywhere once it reaches the wrap width.
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
                            // A line cut inside a word gets a hyphen.
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
        MainForm->players->ShowBalloon(player_id, balloon_lines, text_width, text_height, style);
    }
}

// Creates the emote buffer, freeing the earlier one first.
void SpriteGraphics::CreateEmoteBuffer()
{
    if (emote_buffer_created)
        FreeEmoteBuffer();
    emote_buffer = new EmoteBuffer();
    emote_buffer_created = true;
}

// Frees the emote buffer if one exists.
void SpriteGraphics::FreeEmoteBuffer()
{
    if (emote_buffer_created)
    {
        delete emote_buffer;
        emote_buffer_created = false;
    }
}

// Draws one bitmap layer (armor or hair) into the emote buffer at an offset, with black see-through.
void SpriteGraphics::DrawLayerToEmoteBuffer(int x_offset, int y_offset, int library, int resource_id)
{
    HDC dc;
    HDC CompatibleDC;
    if (emote_buffer_created)
    {
        MainForm->resource_bitmap->Width = 0;
        MainForm->resource_bitmap->Height = 0;
        MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
        MainForm->resource_bitmap->TransparentColor = clBlack;
        MainForm->resource_bitmap->Transparent = true;
        // A canvas Draw by default; with BitBlt drawing configured, TransparentBlt with the same key.
        if (!MainForm->config->GetUseBitBltDraw())
        {
            if (emote_buffer->surface->GetDC(&dc))
            {
                try
                {
                    MainForm->draw_canvas->Handle = dc;
                    MainForm->draw_canvas->Draw(0 - x_offset, 0 - y_offset, MainForm->resource_bitmap);
                    MainForm->draw_canvas->Handle = 0;
                }
                catch (Exception &)
                {
                    MainForm->CountDrawError();
                }
                emote_buffer->surface->ReleaseDC(dc);
            }
        }
        else
        {
            CompatibleDC = CreateCompatibleDC(0);
            SelectObject(CompatibleDC, MainForm->resource_bitmap->Handle);
            if (emote_buffer->surface->GetDC(&dc))
            {
                try
                {
                    TransparentBlt(dc, 0 - x_offset, 0 - y_offset, MainForm->resource_bitmap->Width,
                        MainForm->resource_bitmap->Height, CompatibleDC, 0, 0, MainForm->resource_bitmap->Width,
                        MainForm->resource_bitmap->Height, 0);
                }
                catch (Exception &)
                {
                }
                emote_buffer->surface->ReleaseDC(dc);
            }
            DeleteDC(CompatibleDC);
        }
    }
}

// Draws a hat bitmap into the emote buffer at an offset, see-through in the colour of one fixed pixel of the bitmap.
void SpriteGraphics::DrawLayerToEmoteBufferPixelKeyed(int x_offset, int y_offset, int library, int resource_id)
{
    if (emote_buffer_created)
    {
        MainForm->resource_bitmap->Width = 0;
        MainForm->resource_bitmap->Height = 0;
        MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
        int Pixel = MainForm->resource_bitmap->Canvas->Pixels[1][32];
        BYTE key_red = GetRValue(Pixel);
        BYTE key_blue = GetBValue(Pixel);
        BYTE key_green = GetGValue(Pixel);
        MainForm->resource_bitmap->TransparentColor = (TColor)RGB(key_red, key_green, key_blue);
        MainForm->resource_bitmap->Transparent = true;
        HDC dc;
        // A canvas Draw by default; with BitBlt drawing configured, TransparentBlt keyed to that pixel's colour.
        if (!MainForm->config->GetUseBitBltDraw())
        {
            if (emote_buffer->surface->GetDC(&dc))
            {
                try
                {
                    MainForm->draw_canvas->Handle = dc;
                    MainForm->draw_canvas->Draw(0 - x_offset, 0 - y_offset, MainForm->resource_bitmap);
                    MainForm->draw_canvas->Handle = 0;
                }
                catch (Exception &)
                {
                }
                emote_buffer->surface->ReleaseDC(dc);
            }
        }
        else
        {
            HDC CompatibleDC = CreateCompatibleDC(0);
            SelectObject(CompatibleDC, MainForm->resource_bitmap->Handle);
            if (emote_buffer->surface->GetDC(&dc))
            {
                try
                {
                    TransparentBlt(dc, 0 - x_offset, 0 - y_offset, MainForm->resource_bitmap->Width,
                        MainForm->resource_bitmap->Height, CompatibleDC, 0, 0, MainForm->resource_bitmap->Width,
                        MainForm->resource_bitmap->Height, RGB(key_red, key_green, key_blue));
                }
                catch (Exception &)
                {
                }
                emote_buffer->surface->ReleaseDC(dc);
            }
            DeleteDC(CompatibleDC);
        }
    }
}
