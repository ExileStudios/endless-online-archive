#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Gfx_balloon.h"
#include "Gfx_interface.h"
#include "Gfx_player.h"
#include "Gfx_game.h"
#include "Ctrl_interface.h"
#include "Gfx_playername.h"
#pragma package(smart_init)

Player::Player()
{
    name = "";
    player_id = PlayerId_Unset;
    record_value_d = 0;
    direction = FacingDirection_Down;
    slot_state = 0;
    effect_start = 0;
    effect_running = 0;
    pending_effect = -1;
    effect_frame = 0;
    effect_id = 0;
    health_bar_start = 0;
    health_bar_shown = 0;
    damage_start = 0;
    damage_shown = 0;
    emote_start = false;
    emoting = 0;
    walk_start = 0;
    walking = 0;
    jump_start = false;
    jumping = false;
    cast_start = false;
    casting = 0;
    attack_start = 0;
    attacking = 0;
    silent_attack = 0;
    dying = 0;
    hiding = false;
    unhiding = false;
    stand_up_start = false;
    chair_sit_start = false;
    floor_sit_start = false;
    recenter_view = 0;
    sitting_on_chair = false;
    sitting_on_floor = false;
    balloon_timer = 0;
    balloon_shown = 0;
    out_of_range = 0;
    graphics_pending = 1;
    reload_requested = 0;
    load_progress = 0;
    sheets_cleared = false;
    walk_sheets_loaded = false;
    walk_tail_sheets_loaded = false;
    stand_sheet_loaded = false;
    sit_sheets_loaded = false;
    attack_sheets_loaded = false;
    extra_sheets_loaded = false;
    cast_sheets_loaded = false;
    emote_sheets_loaded = false;
    hover_label_shown = 0;
    text_label_shown = false;
    text_label_shown = false;
    level_up_pending = false;
    hidden = 0;
    badge_level = 0;
    health_bar_timer = -1;
    damage_timer = -1;
    resources = 0;
}

// A player at the tile with their looks; start_pending marks their cells to be drawn.
Player::Player(int tile_x, int tile_y, String player_name, int player_gender, int player_skin, int player_hair_style,
    int player_hair_color, char start_pending)
{
    x = tile_x;
    y = tile_y;
    direction = FacingDirection_Down;
    slot_state = 0;
    name = player_name;
    gender = player_gender;
    skin = player_skin;
    hair_style = player_hair_style;
    hair_color = player_hair_color;
    effect_start = 0;
    effect_running = 0;
    pending_effect = -1;
    effect_frame = 0;
    effect_id = 0;
    health_bar_start = 0;
    health_bar_shown = 0;
    damage_start = 0;
    damage_shown = 0;
    emote_start = false;
    emoting = 0;
    walk_start = 0;
    walking = 0;
    jump_start = false;
    jumping = false;
    cast_start = false;
    casting = 0;
    attack_start = 0;
    attacking = 0;
    dying = 0;
    unhiding = false;
    hiding = false;
    stand_up_start = false;
    chair_sit_start = false;
    floor_sit_start = false;
    recenter_view = 0;
    sitting_on_chair = false;
    sitting_on_floor = false;
    balloon_timer = 0;
    balloon_shown = 0;
    out_of_range = 0;
    graphics_pending = start_pending;
    reload_requested = 0;
    load_progress = 0;
    sheets_cleared = false;
    walk_sheets_loaded = false;
    walk_tail_sheets_loaded = false;
    stand_sheet_loaded = false;
    sit_sheets_loaded = false;
    attack_sheets_loaded = false;
    extra_sheets_loaded = false;
    cast_sheets_loaded = false;
    emote_sheets_loaded = false;
    hover_label_shown = 0;
    text_label_shown = false;
    level_up_pending = false;
    hidden = 0;
    badge_level = 0;
    health_bar_timer = -1;
    damage_timer = -1;
}

Player::~Player()
{
}

// Opens a speech balloon over the player, replacing an open one; longer text stays up longer.
void Player::ShowBalloon(String *lines, int width, int height, int tinted)
{
    if (balloon_shown)
    {
        CloseBalloon();
        Sleep(1);
    }
    balloon = new Balloon(width, height, lines, tinted);
    int text_length = 0;
    for (int i = 0; i <= 5; i++)
        text_length += lines[i].Length();
    balloon_width = width;
    balloon_height = height;
    balloon_shown = 1;
    balloon_timer = text_length / 3 + 24;
    redraw = 1;
}

// The screen position adjusted for the current step, jump, sitting pose or attack lunge.
Windows::TPoint Player::GetAnimatedPosition(int screen_x, int screen_y)
{
    TPoint position;
    position.x = screen_x;
    position.y = screen_y;
    // The player already stands on the new tile; the offset draws them back toward the old one, less each frame.
    if (walking)
    {
        if (walk_direction == FacingDirection_Left)
        {
            position.x = position.x + 32 - (8 * walk_frame + 8);
            position.y = position.y + 16 - (4 * walk_frame + 4);
        }
        if (walk_direction == FacingDirection_Up)
        {
            position.x = 8 * walk_frame + position.x - 24;
            position.y = position.y + 16 - (4 * walk_frame + 4);
        }
        if (walk_direction == FacingDirection_Right)
        {
            position.x = 8 * walk_frame + position.x - 24;
            position.y = 4 * walk_frame + position.y - 12;
        }
        if (walk_direction == FacingDirection_Down)
        {
            position.x = position.x + 32 - (8 * walk_frame + 8);
            position.y = 4 * walk_frame + position.y - 12;
        }
    }
    if (jumping)
    {
        if (jump_direction == FacingDirection_Left)
        {
            position.x = position.x + 32 - (8 * jump_frame + 8);
            position.y = position.y + 16 - (4 * jump_frame + 4);
        }
        if (jump_direction == FacingDirection_Up)
        {
            position.x = 8 * jump_frame + position.x - 24;
            position.y = position.y + 16 - (4 * jump_frame + 4);
        }
        if (jump_direction == FacingDirection_Right)
        {
            position.x = 8 * jump_frame + position.x - 24;
            position.y = 4 * jump_frame + position.y - 12;
        }
        if (jump_direction == FacingDirection_Down)
        {
            position.x = position.x + 32 - (8 * jump_frame + 8);
            position.y = 4 * jump_frame + position.y - 12;
        }
        // A jump also lifts the player, highest on the middle frame.
        if (!jump_frame || jump_frame == 2)
            position.y = position.y - 14;
        if (jump_frame == 1)
            position.y = position.y - 22;
    }
    if (sitting_on_chair)
    {
        int chair_facing = direction;
        if (!(chair_facing != FacingDirection_Down && chair_facing != FacingDirection_Right))
            position.y += 14;
        else
            position.y += 16;
    }
    if (sitting_on_floor)
    {
        int floor_facing = direction;
        if (!(floor_facing != FacingDirection_Down && floor_facing != FacingDirection_Right))
            position.y += 19;
        else
        {
            if (floor_facing == FacingDirection_Left)
                position.x += 3;
            if (floor_facing == FacingDirection_Up)
                position.x -= 3;
            position.y += 21;
        }
    }
    if (attacking)
    {
        int current_attack_frame = attack_frame;
        // The lunge toward the facing direction covers frames 1 to 3.
        if (current_attack_frame >= 1 && current_attack_frame <= 3)
        {
            if (MainForm->weapon_mapper->IsRangedWeapon(weapon))
            {
                if (attack_frame != 3)
                {
                    if (attack_direction == FacingDirection_Down)
                    {
                        position.x -= gender + 2;
                        position.y += gender;
                    }
                    if (attack_direction == FacingDirection_Left)
                        position.x = position.x - 3;
                    if (attack_direction == FacingDirection_Up)
                        position.x += 3;
                    if (attack_direction == FacingDirection_Right)
                    {
                        position.x = position.x + (gender + 2);
                        position.y += gender;
                    }
                }
            }
            else
            {
                if (attack_direction == FacingDirection_Down)
                {
                    position.x = position.x - 7;
                    position.y += 4;
                }
                if (attack_direction == FacingDirection_Left)
                    position.x -= 7 - gender;
                if (attack_direction == FacingDirection_Up)
                    position.x += 7 - gender;
                if (attack_direction == FacingDirection_Right)
                {
                    position.x += 7;
                    position.y += 4;
                }
            }
        }
    }
    return position;
}

// Draws the current emote frame above the player, more faded on its later steps.
void Player::DrawEmote()
{
    int screen_x = 32 * x - 32 * y + MainForm->map->GetScrollOffsetX() + 306 - 22;
    int screen_y = 16 * x + 16 * y + MainForm->map->GetScrollOffsetY() + 90 - 48;
    if (MainForm->animation->interface_controller->is_shaking)
        screen_x += MainForm->animation->interface_controller->shake_offset;
    if (emote_step < 2)
        MainForm->gfx->game->BlitClippedAlpha(GetAnimatedPosition(screen_x, screen_y).x,
            GetAnimatedPosition(screen_x, screen_y).y, MainForm->gfx->interface_graphics->emote_sheet,
            Gfx_interface_GetEmoteFrameRect(MainForm->gfx->interface_graphics, emote_first_frame + emote_frame - 1),
            170);
    if (emote_step == 2)
        MainForm->gfx->game->BlitClippedAlpha(GetAnimatedPosition(screen_x, screen_y).x,
            GetAnimatedPosition(screen_x, screen_y).y, MainForm->gfx->interface_graphics->emote_sheet,
            Gfx_interface_GetEmoteFrameRect(MainForm->gfx->interface_graphics, emote_first_frame + emote_frame - 1),
            150);
    if (emote_step == 3)
        MainForm->gfx->game->BlitClippedAlpha(GetAnimatedPosition(screen_x, screen_y).x,
            GetAnimatedPosition(screen_x, screen_y).y, MainForm->gfx->interface_graphics->emote_sheet,
            Gfx_interface_GetEmoteFrameRect(MainForm->gfx->interface_graphics, emote_first_frame + emote_frame - 1),
            130);
}

// Draws the balloon centred above the player.
void Player::DrawBalloon()
{
    int screen_x = TILE_HALF_WIDTH * x - TILE_HALF_WIDTH * y + MainForm->map->GetScrollOffsetX() + 302 - balloon_width / 2;
    int screen_y = 16 * x + 16 * y + MainForm->map->GetScrollOffsetY() + 76 - balloon_height - 2 * gender;
    if (MainForm->animation->interface_controller->is_shaking)
        screen_x += MainForm->animation->interface_controller->shake_offset;
    MainForm->gfx->game->BlitClippedAlpha(GetAnimatedPosition(screen_x, screen_y).x,
        GetAnimatedPosition(screen_x, screen_y).y, balloon->fill_surface, balloon->bubble_rect, 160);
    MainForm->gfx->game->BlitClipped(GetAnimatedPosition(screen_x, screen_y).x,
        GetAnimatedPosition(screen_x, screen_y).y, balloon->text_surface, balloon->bubble_rect);
}

// Draws the shared name label centred above the player.
void Player::DrawHoverLabel()
{
    int screen_x = 32 * x - 32 * y + MainForm->map->GetScrollOffsetX() + 306 -
        (MainForm->gfx->hover_label->text_width - 4) / 2;
    int screen_y = 16 * x + 16 * y + MainForm->map->GetScrollOffsetY() + 90 - MainForm->gfx->hover_label->text_height -
        2 * gender;
    if (MainForm->animation->interface_controller->is_shaking)
        screen_x += MainForm->animation->interface_controller->shake_offset;
    MainForm->gfx->game->BlitClipped(GetAnimatedPosition(screen_x, screen_y).x,
        GetAnimatedPosition(screen_x, screen_y).y, MainForm->gfx->hover_label->surface,
        MainForm->gfx->hover_label->surface_rect);
}

// Draws the player's own text label above them, when their cells exist.
void Player::DrawTextLabel()
{
    if (resources)
    {
        int screen_x = 32 * x - 32 * y + MainForm->map->GetScrollOffsetX() + 308 -
            (resources->name_tag_text_rect.right - 4) / 2;
        int screen_y = 16 * x + 16 * y + MainForm->map->GetScrollOffsetY() + 84 - resources->name_tag_text_rect.bottom -
            2 * gender;
        if (MainForm->animation->interface_controller->is_shaking)
            screen_x += MainForm->animation->interface_controller->shake_offset;
        MainForm->gfx->game->BlitClipped(GetAnimatedPosition(screen_x, screen_y).x,
            GetAnimatedPosition(screen_x, screen_y).y, resources->name_tag_surface, resources->name_tag_text_rect);
    }
}

// Closes the player's balloon if one is open, and asks for a redraw.
DEFINE_CLOSE_BALLOON(Player)

void Player::CreateResources()
{
    resources = new PlayerResources();
}

void Player::FreeResources()
{
    if (resources)
    {
        delete resources;
        resources = 0;
    }
}
