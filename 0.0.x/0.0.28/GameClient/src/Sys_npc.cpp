#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Gfx_balloon.h"
#include "Gfx_npc.h"
#include "Gfx_game.h"
#include "Ctrl_interface.h"
#pragma package(smart_init)

Npc::Npc()
{
    index = 0;
    direction = FacingDirection_Down;
    hidden = 0;
    health_bar_start = 0;
    health_bar_shown = 0;
    damage_start = 0;
    damage_shown = 0;
    effect_start = 0;
    effect_running = 0;
    effect_frame = 0;
    effect_id = 0;
    walk_start = 0;
    walking = 0;
    alt_attack_start = false;
    alt_attacking = false;
    attack_start = 0;
    attacking = 0;
    out_of_range = 0;
    dying = 0;
    graphics_pending = 1;
    reload_requested = 0;
    load_stage = 0;
    label_shown = 0;
    idle_frame = 0;
    balloon_timer = 0;
    queued_balloon_count = 0;
    balloon_shown = 0;
    frame_width = 0;
    frame_height = 0;
    health_bar_timer = -1;
    damage_timer = -1;
}

// An npc at the tile, facing direction, marked pending.
Npc::Npc(int map_index, int record_id, int graphic_id, String display_name, int tile_x, int tile_y, int facing)
{
    index = map_index;
    npc_id = record_id;
    direction = FacingDirection_Down;
    hidden = 0;
    name = display_name;
    graphic = graphic_id;
    x = tile_x;
    y = tile_y;
    direction = facing;
    health_bar_start = 0;
    health_bar_shown = 0;
    damage_start = 0;
    damage_shown = 0;
    effect_start = 0;
    effect_running = 0;
    effect_frame = 0;
    effect_id = 0;
    walk_start = 0;
    walking = 0;
    alt_attack_start = false;
    alt_attacking = false;
    attack_start = 0;
    attacking = 0;
    out_of_range = 0;
    dying = 0;
    graphics_pending = 1;
    reload_requested = 0;
    label_shown = 0;
    load_stage = 0;
    label_shown = 0;
    idle_frame = 0;
    balloon_timer = 0;
    balloon_shown = 0;
    frame_width = 0;
    frame_height = 0;
    health_bar_timer = -1;
    damage_timer = -1;
}

void Npc::CreateResources()
{
    resources = new NpcResources();
}

// The screen position adjusted for the current step or attack lunge.
Windows::TPoint Npc::GetAnimatedPosition(Npc *self, int screen_x, int screen_y)
{
    TPoint position;
    position.x = screen_x;
    position.y = screen_y;
    if (self->walking)
    {
        if (self->walk_direction == FacingDirection_Left)
        {
            position.x = position.x + 32 - (8 * self->walk_frame + 8);
            position.y = position.y + 16 - (4 * self->walk_frame + 4);
        }
        if (self->walk_direction == FacingDirection_Up)
        {
            position.x = 8 * self->walk_frame + position.x - 24;
            position.y = position.y + 16 - (4 * self->walk_frame + 4);
        }
        if (self->walk_direction == FacingDirection_Right)
        {
            position.x = 8 * self->walk_frame + position.x - 24;
            position.y = 4 * self->walk_frame + position.y - 12;
        }
        if (self->walk_direction == FacingDirection_Down)
        {
            position.x = position.x + 32 - (8 * self->walk_frame + 8);
            position.y = 4 * self->walk_frame + position.y - 12;
        }
    }
    if (self->attacking)
    {
        int current_attack_frame = self->attack_frame;
        if (current_attack_frame >= 1 && current_attack_frame <= 3)
        {
            if (self->attack_direction == FacingDirection_Down)
            {
                position.x = position.x - 7;
                position.y += 4;
            }
            if (self->attack_direction == FacingDirection_Left)
                position.x = position.x - 7;
            if (self->attack_direction == FacingDirection_Up)
                position.x += 7;
            if (self->attack_direction == FacingDirection_Right)
            {
                position.x += 7;
                position.y += 4;
            }
        }
    }
    return position;
}

// Opens a speech balloon over the npc, or queues up to five while one is showing.
void Npc::ShowBalloon(String *lines, int width, int height, int tinted)
{
    if (balloon_timer < 1)
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
    else if (queued_balloon_count < 5)
    {
        queued_balloon_lines[0][queued_balloon_count] = *lines;
        queued_balloon_lines[1][queued_balloon_count] = lines[1];
        queued_balloon_lines[2][queued_balloon_count] = lines[2];
        queued_balloon_lines[3][queued_balloon_count] = lines[3];
        queued_balloon_lines[4][queued_balloon_count] = lines[4];
        queued_balloon_lines[5][queued_balloon_count] = lines[5];
        queued_balloon_widths[queued_balloon_count] = width; queued_balloon_heights[queued_balloon_count] = height;
        queued_balloon_tinted[queued_balloon_count] = tinted;
        queued_balloon_count++;
    }
}

// Draws the balloon centred above the npc, raised by the head height of its graphic.
void Npc::DrawBalloon()
{
    int screen_x = TILE_HALF_WIDTH * x - TILE_HALF_WIDTH * y + MainForm->map->GetScrollOffsetX() + 302 - balloon_width / 2;
    int screen_y = 16 * x + 16 * y + MainForm->map->GetScrollOffsetY() + 112 - balloon_height;
    screen_y -= MainForm->npcs->head_heights[graphic - 1];
    if (MainForm->animation->interface_controller->is_shaking)
        screen_x += MainForm->animation->interface_controller->shake_offset;
    MainForm->gfx->game->BlitClippedAlpha(GetAnimatedPosition(this, screen_x, screen_y).x,
        GetAnimatedPosition(this, screen_x, screen_y).y, balloon->fill_surface, balloon->bubble_rect, 160);
    MainForm->gfx->game->BlitClipped(GetAnimatedPosition(this, screen_x, screen_y).x,
        GetAnimatedPosition(this, screen_x, screen_y).y, balloon->text_surface, balloon->bubble_rect);
}

// Closes the npc's balloon if one is open, and asks for a redraw.
DEFINE_CLOSE_BALLOON(Npc)

void Npc::FreeResources()
{
    delete resources;
}

Npc::~Npc()
{
}
