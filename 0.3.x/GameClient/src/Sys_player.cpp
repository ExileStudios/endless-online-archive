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

// An empty player: no id or name, facing down, with every animation and effect stopped.
Player::Player()
{
    balloon = 0;
    name = "";
    player_id = PlayerId_Unset;
    record_value_d = 0;
    direction = FacingDirection_Down;
    draw_group = CharacterSlotState_Empty;
    effect_start = false;
    effect_running = false;
    pending_effect = -1;
    effect_frame = 0;
    effect_id = SpellEffect_None;
    health_bar_start = false;
    health_bar_shown = false;
    damage_start = false;
    damage_shown = false;
    emote_start = false;
    emoting = false;
    walk_start = false;
    walking = false;
    jump_start = false;
    jumping = false;
    cast_start = false;
    casting = false;
    attack_start = false;
    attacking = false;
    silent_attack = false;
    dying = false;
    hiding = false;
    unhiding = false;
    stand_up_start = false;
    chair_sit_start = false;
    floor_sit_start = false;
    recenter_view = false;
    sitting_on_chair = false;
    sitting_on_floor = false;
    balloon_timer = 0;
    balloon_shown = false;
    out_of_range = false;
    graphics_pending = true;
    reload_requested = false;
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
    hover_label_shown = false;
    text_label_shown = false;
    text_label_shown = false;
    level_up_pending = false;
    hidden = false;
    badge_level = 0;
    health_bar_timer = -1;
    damage_timer = -1;
    resources = 0;
}

// A player at the tile with their looks; start_pending marks their cells to be drawn.
Player::Player(int tile_x, int tile_y, String player_name, int player_gender, int player_skin, int player_hair_style,
    int player_hair_color, char start_pending)
{
    balloon = 0;
    x = tile_x;
    y = tile_y;
    direction = FacingDirection_Down;
    draw_group = CharacterSlotState_Empty;
    name = player_name;
    gender = player_gender;
    skin = player_skin;
    hair_style = player_hair_style;
    hair_color = player_hair_color;
    effect_start = false;
    effect_running = false;
    pending_effect = -1;
    effect_frame = 0;
    effect_id = SpellEffect_None;
    health_bar_start = false;
    health_bar_shown = false;
    damage_start = false;
    damage_shown = false;
    emote_start = false;
    emoting = false;
    walk_start = false;
    walking = false;
    jump_start = false;
    jumping = false;
    cast_start = false;
    casting = false;
    attack_start = false;
    attacking = false;
    dying = false;
    unhiding = false;
    hiding = false;
    stand_up_start = false;
    chair_sit_start = false;
    floor_sit_start = false;
    recenter_view = false;
    sitting_on_chair = false;
    sitting_on_floor = false;
    balloon_timer = 0;
    balloon_shown = false;
    out_of_range = false;
    graphics_pending = start_pending;
    reload_requested = false;
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
    hover_label_shown = false;
    text_label_shown = false;
    level_up_pending = false;
    hidden = false;
    badge_level = 0;
    health_bar_timer = -1;
    damage_timer = -1;
}

Player::~Player()
{
}

// The screen position adjusted for the current step, jump, sitting pose or attack lunge.
Windows::TPoint Player::GetAnimatedPosition(int screen_x, int screen_y)
{
    TPoint position;
    position.x = screen_x;
    position.y = screen_y;
    // Already on the new tile, the player is drawn back toward the old one, less each frame.
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
        if (jump_frame == 0 || jump_frame == 2)
            position.y = position.y - 14;
        if (jump_frame == 1)
            position.y = position.y - 22;
    }
    if (sitting_on_chair)
    {
        if (direction == FacingDirection_Down || direction == FacingDirection_Right)
            position.y += 14;
        else
            position.y += 16;
    }
    if (sitting_on_floor)
    {
        if (direction == FacingDirection_Down || direction == FacingDirection_Right)
            position.y += 19;
        else
        {
            if (direction == FacingDirection_Left)
                position.x += 3;
            if (direction == FacingDirection_Up)
                position.x -= 3;
            position.y += 21;
        }
    }
    if (attacking)
    {
        int current_attack_frame = attack_frame;
        // The lunge toward the facing direction covers frames 1 to 3.
        if (attack_frame >= 1 && attack_frame <= 3)
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
                        position.x = gender + position.x + 2;
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
            MainForm->gfx->interface_graphics->GetWideIconRect(emote_first_frame + emote_frame - 1), 170);
    if (emote_step == 2)
        MainForm->gfx->game->BlitClippedAlpha(GetAnimatedPosition(screen_x, screen_y).x,
            GetAnimatedPosition(screen_x, screen_y).y, MainForm->gfx->interface_graphics->emote_sheet,
            MainForm->gfx->interface_graphics->GetWideIconRect(emote_first_frame + emote_frame - 1), 150);
    if (emote_step == 3)
        MainForm->gfx->game->BlitClippedAlpha(GetAnimatedPosition(screen_x, screen_y).x,
            GetAnimatedPosition(screen_x, screen_y).y, MainForm->gfx->interface_graphics->emote_sheet,
            MainForm->gfx->interface_graphics->GetWideIconRect(emote_first_frame + emote_frame - 1), 130);
}

// Shows a balloon of the speaker's text for 24 ticks plus one for every 3 characters.
void Player::ShowBalloon(String speaker, String text, int style)
{
    if (balloon)
    {
        CloseBalloon();
        delete balloon;
    }
    balloon = new Balloon(speaker, text, style);
    balloon_timer = (short)(text.Length() / 3) + 24;
    balloon_shown = true;
    redraw = true;
}

void Player::HideBalloon()
{
    CloseBalloon();
}

// Draws the player's balloon over their head, shaken with the screen.
void Player::DrawBalloon()
{
    int screen_x = 32 * x - 32 * y + MainForm->map->GetScrollOffsetX() + 302 - balloon->text_width / 2;
    int balloon_y = 16 * x + 16 * y + MainForm->map->GetScrollOffsetY() + 76 - balloon->text_height - 2 * gender;
    if (MainForm->animation->interface_controller->is_shaking)
        screen_x += MainForm->animation->interface_controller->shake_offset;
    if (balloon)
        balloon->Draw(GetAnimatedPosition(screen_x, balloon_y).x, GetAnimatedPosition(screen_x, balloon_y).y);
}

void Player::CloseBalloon()
{
    balloon_shown = false;
    balloon_timer = 0;
    if (balloon)
    {
        delete balloon;
        balloon = 0;
    }
    redraw = true;
}

// Draws the shared name label centred above the player.
void Player::DrawHoverLabel()
{
    int screen_x =
        32 * x - 32 * y + MainForm->map->GetScrollOffsetX() + 306 - (MainForm->gfx->hover_label->text_width - 4) / 2;
    int screen_y =
        16 * x + 16 * y + MainForm->map->GetScrollOffsetY() + 90 - MainForm->gfx->hover_label->text_height - 2 * gender;
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
        int screen_x =
            32 * x - 32 * y + MainForm->map->GetScrollOffsetX() + 308 - (resources->name_tag_text_rect.right - 4) / 2;
        int screen_y =
            16 * x + 16 * y + MainForm->map->GetScrollOffsetY() + 84 - resources->name_tag_text_rect.bottom -
                2 * gender;
        if (MainForm->animation->interface_controller->is_shaking)
            screen_x += MainForm->animation->interface_controller->shake_offset;
        MainForm->gfx->game->BlitClipped(GetAnimatedPosition(screen_x, screen_y).x,
            GetAnimatedPosition(screen_x, screen_y).y, resources->name_tag_surface, resources->name_tag_text_rect);
    }
}

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
