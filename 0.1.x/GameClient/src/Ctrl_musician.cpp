#include <vcl.h>
#pragma hdrstop

#include "Ctrl_musician.h"
#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Packetoperations.h"

#pragma package(smart_init)

MusicianController::MusicianController()
{
    window_x = 15;
    window_y = 176;
    is_visible = false;
    is_dragging = false;
    hovered_note_col = -1;
    hovered_note_row = -1;
    unused_index = -1;
    note_cooldown = -1;
}

MusicianController::~MusicianController()
{
}

// Counts down the wait before the next note.
void MusicianController::Tick()
{
    if (note_cooldown > 0)
        note_cooldown--;
}

// Shows the musician window at its starting place.
void MusicianController::Open()
{
    MainForm->gfx->CreateMusicianGraphics();
    MainForm->gfx->LoadMusicianGraphics();
    window_x = 15;
    window_y = 176;
    is_visible = true;
    MainForm->redraw_needed = true;
}

// Hides the window and frees its graphics.
void MusicianController::Close()
{
    MainForm->gfx->FreeMusicianGraphics();
    is_visible = false;
    MainForm->redraw_needed = true;
}

// A click on the note grid plays that note on a held instrument and sends it, with a short wait before the next;
// any other click in the window starts moving it.
void MusicianController::MouseDown(int x, int y, int button)
{
    int row_base;
    int note_col;
    char handled = false;
    if (x >= window_x + 16 && x <= window_x + 253 && y >= window_y + 15 && y <= window_y + 73 && note_cooldown <= 0 &&
        MainForm->net_client->CanAct())
    {
        int instrument_id = MainForm->players->GetWeapon(MainForm->net_client->GetPlayerId());
        // Only these two weapons are instruments
        if (instrument_id == 49 || instrument_id == 50)
        {
            note_col = (x - window_x - 14) / 20;
            row_base = 12 * ((y - window_y - 15) / MUSICIAN_CELL_SIZE);
            note_col++;
            // Notes count from one, twelve to a row
            int note = note_col + row_base;
            if (note > 36)
                note = 36;
            MainForm->sfx->PlayInstrumentNote(instrument_id, note);
            MainForm->net_client->sender->SendPlayInstrument(instrument_id, note);
            MainForm->animation->game->AttackForward(true);
            note_cooldown = 2;
            handled = true;
        }
    }
    if (!handled)
    {
        if (x >= window_x && x <= window_x + 270 && y >= window_y && y <= window_y + 125)
        {
            is_dragging = true;
            drag_offset_x = x - window_x;
            drag_offset_y = y - window_y;
        }
    }
}

// Moves a dragged window, kept on screen, and tracks the hovered note and the close button.
void MusicianController::MouseMove(int x, int y)
{
    int note_row;
    if (is_dragging)
    {
        window_x = x - drag_offset_x;
        window_y = y - drag_offset_y;
        if (window_x <= 10)
            window_x = 10;
        if (window_y <= 8)
            window_y = 8;
        if (window_x >= 358)
            window_x = 358;
        if (window_y >= 184)
            window_y = 184;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 16 && x <= window_x + 253 && y >= window_y + 15 && y <= window_y + 73)
    {
        int note_col = (x - window_x - 14) / 20;
        note_row = (y - window_y - 15) / MUSICIAN_CELL_SIZE;
        if (note_col != hovered_note_col || note_row != hovered_note_row)
        {
            hovered_note_col = note_col;
            hovered_note_row = note_row;
            MainForm->redraw_needed = true;
        }
    }
    else if (hovered_note_col >= 0)
    {
        hovered_note_col = -1;
        hovered_note_row = -1;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 91 && x <= window_x + 181 && y >= window_y + 82 && y <= window_y + 108)
    {
        if (!close_hovered)
        {
            close_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else if (close_hovered)
    {
        close_hovered = false;
        MainForm->redraw_needed = true;
    }
}

// Ends moving the window.
void MusicianController::MouseUp()
{
    is_dragging = false;
}
