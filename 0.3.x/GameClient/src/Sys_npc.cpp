#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Gfx_balloon.h"
#include "Gfx_npc.h"
#include "Gfx_game.h"
#include "Ctrl_interface.h"

#pragma package(smart_init)

// An empty npc at the map index, with every animation stopped.
Npc::Npc(int map_index)
{
    balloon = 0;
    index = map_index;
    bar_index = map_index;
    npc_id = 1;
    hidden = false;
    walk_start = false;
    walking = false;
    effect_start = false;
    effect_running = false;
    effect_frame = 0;
    effect_id = SpellEffect_None;
    dying = false;
    killed_by_hit = false;
    death_countdown = 0;
    label_shown = false;
    idle_frame = 0;
    queued_balloon_count = 0;
    balloon_timer = 0;
    balloon_shown = false;
    frame_width = 0;
    frame_height = 0;
    damage_start = false;
    damage_shown = false;
    damage_timer = -1;
    health_bar_start = false;
    health_bar_shown = false;
    health_bar_width = 0;
    health_bar_timer = -1;
}

// An npc at the tile, facing direction, marked pending.
Npc::Npc(int map_index, int record_id, int graphic_id, String display_name, int tile_x, int tile_y, int facing)
{
    balloon = 0;
    index = map_index;
    bar_index = map_index;
    npc_id = record_id;
    hidden = false;
    direction = FacingDirection_Down;
    name = display_name;
    graphic = graphic_id;
    x = tile_x;
    y = tile_y;
    direction = facing;
    effect_start = false;
    effect_running = false;
    effect_frame = 0;
    effect_id = SpellEffect_None;
    walk_start = false;
    walking = false;
    alt_attack_start = false;
    alt_attacking = false;
    attack_start = false;
    attacking = false;
    dying = false;
    killed_by_hit = false;
    death_countdown = 0;
    out_of_range = true;
    graphics_pending = false;
    load_stage = 0;
    reload_requested = false;
    label_shown = false;
    idle_frame = 0;
    queued_balloon_count = 0;
    balloon_timer = 0;
    balloon_shown = false;
    frame_width = 0;
    frame_height = 0;
    damage_start = false;
    damage_shown = false;
    damage_timer = -1;
    health_bar_start = false;
    health_bar_shown = false;
    health_bar_width = 0;
    health_bar_timer = -1;
}

Npc::~Npc()
{
}

// The screen position adjusted for the current step or attack lunge.
TPoint Npc::GetAnimatedPosition(int screen_x, int screen_y)
{
    TPoint position;
    position.x = screen_x;
    position.y = screen_y;
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
    // Frames 1 to 3 of an attack lean the npc 7 pixels toward its target.
    if (attacking)
    {
        if (attack_frame >= 1 && attack_frame <= 3)
        {
            if (attack_direction == FacingDirection_Down)
            {
                position.x = position.x - 7;
                position.y += 4;
            }
            if (attack_direction == FacingDirection_Left)
                position.x = position.x - 7;
            if (attack_direction == FacingDirection_Up)
                position.x += 7;
            if (attack_direction == FacingDirection_Right)
            {
                position.x += 7;
                position.y += 4;
            }
        }
    }
    return position;
}

// Shows a speech balloon over the npc for a time by the text's length; while one is shown, up to six more wait.
void Npc::ShowBalloon(String speaker, String text, int style)
{
    if (balloon_timer < 1)
    {
        if (balloon)
        {
            FreeBalloon();
            delete balloon;
        }
        balloon = new Balloon(speaker, text, style);
        balloon_timer = (short)(text.Length() / 3) + 24;
        balloon_shown = true;
        redraw = true;
        return;
    }
    if (queued_balloon_count < 6)
    {
        queued_balloon_lines[queued_balloon_count] = text;
        queued_balloon_styles[queued_balloon_count] = style;
        queued_balloon_count++;
    }
}

// Shows the npc's next queued balloon and moves the rest of the queue up.
void Npc::ShowNextBalloon()
{
    FreeBalloon();
    if (queued_balloon_count > 0)
    {
        ShowBalloon(name, queued_balloon_lines[0], queued_balloon_styles[0]);
        for (int i = 0; i < 6; i++)
        {
            queued_balloon_lines[i] = queued_balloon_lines[i + 1];
            queued_balloon_styles[i] = queued_balloon_styles[i + 1];
        }
        queued_balloon_count--;
    }
}

// Draws the npc's balloon over its head, shaken with the screen.
void Npc::DrawBalloon()
{
    int balloon_y;
    if (balloon)
    {
        int balloon_x =
            TILE_HALF_WIDTH * x - TILE_HALF_WIDTH * y + MainForm->map->GetScrollOffsetX() + 302 -
                balloon->text_width / 2;
        balloon_y = 16 * x + 16 * y + MainForm->map->GetScrollOffsetY() + 112 - balloon->text_height;
        balloon_y -= MainForm->npcs->head_heights[graphic - 1];
        if (MainForm->animation->interface_controller->is_shaking)
            balloon_x += MainForm->animation->interface_controller->shake_offset;
        if (balloon)
            balloon->Draw(GetAnimatedPosition(balloon_x, balloon_y).x, GetAnimatedPosition(balloon_x, balloon_y).y);
    }
}

void Npc::FreeBalloon()
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
