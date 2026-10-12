#include <vcl.h>
#pragma hdrstop

#include "Ctrl_jukebox.h"
#include "Tmainform.h"
#include "Ctrl_guild.h"
#include "Ctrl_game.h"
#include "Ctrl_interface.h"
#include "Ctrl_status.h"
#pragma package(smart_init)

JukeboxController::JukeboxController()
{
    window_x = 185;
    window_y = 53;
    is_visible = 0;
    open_reply_number = -1;
}

JukeboxController::~JukeboxController()
{
}

// Counts down the song in play and ends it when its time runs out.
void JukeboxController::Tick()
{
    if (is_playing)
    {
        play_ticks_left--;
        if (play_ticks_left <= 0)
        {
            is_playing = 0;
            MainForm->redraw_needed = true;
        }
    }
}

// Shows the jukebox from the server's reply, with the song in play and who asked for it when one is playing.
void JukeboxController::Open(int reply_number, char playing_flag, int reserved, String requester)
{
    MainForm->gfx->CreateJukeboxGraphics();
    MainForm->gfx->LoadJukeboxGraphics();
    open_reply_number = reply_number;
    is_playing = playing_flag;
    requester_name = requester;
    if (is_playing)
        play_ticks_left = JUKEBOX_PLAY_TICKS;
    selected_song = 0;
    is_visible = 1;
    MainForm->redraw_needed = true;
}

// Hides the jukebox and frees its graphics.
void JukeboxController::Close()
{
    MainForm->gfx->FreeJukeboxGraphics();
    is_visible = 0;
    MainForm->redraw_needed = true;
}

// A click on the first row picks the next song, and one on the second row asks to play it for the fee or says why
// not; any other click in the window starts moving it.
void JukeboxController::MouseDown(int x, int y, int button)
{
    char handled = 0;
    if (button == 0 || button == 1)
    {
        if (y > window_y + 50 && y < window_y + 170 && window_x + 17 < x)
        {
            if (window_x + 252 > x)
            {
                int row = (y - window_y - 52) / NPC_LIST_ROW_HEIGHT;
                if (!row)
                {
                    selected_song++;
                    // After the last song the choice goes back to the first
                    if (selected_song > 19)
                        selected_song = 0;
                    handled = 1;
                }
                if (!--row)
                {
                    handled = 1;
                    if (MainForm->animation->interface_controller->status->gold >= 25)
                    {
                        if (!is_playing)
                        {
                            String confirm_text = MainForm->strings->GetText(GameText_RequestThisSongFor);
                            confirm_text = confirm_text + " ";
                            confirm_text = confirm_text + IntToStr(25);
                            confirm_text = confirm_text + " ";
                            confirm_text = confirm_text + LowerCase(MainForm->item_values->GetName(1));
                            confirm_text = confirm_text + "?";
                            // A yes in this box sends the request for the picked song
                            MainForm->animation->game->ShowMessageBox(3, ConfirmAction_JukeboxRequest,
                                MainForm->strings->GetText(GameText_RequestSong), confirm_text);
                            MainForm->edits->caret_enabled = false;
                        }
                        else
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                // A song is already playing
                                MainForm->strings->GetDialogText(DialogText_JukeboxBusy, false),
                                MainForm->strings->GetDialogText(DialogText_JukeboxBusy, true));
                            MainForm->edits->caret_enabled = false;
                        }
                    }
                    else
                    {
                        String no_gold_text = MainForm->strings->GetDialogText(DialogText_WarningEnough, true) + " " +
                            LowerCase(MainForm->item_values->GetName(1));
                        MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_WarningEnough, false), no_gold_text);
                        MainForm->edits->caret_enabled = false;
                    }
                }
            }
        }
    }
    if (!handled)
    {
        if (window_x <= x && window_x + 270 >= x && y >= window_y && y <= window_y + 202)
        {
            is_dragging = 1;
            drag_offset_x = x - window_x;
            drag_offset_y = y - window_y;
        }
    }
}

// Moves a dragged window, kept on screen, and tracks the hovered row and the close button.
void JukeboxController::MouseMove(int x, int y)
{
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
        if (window_y >= 107)
            window_y = 107;
        MainForm->redraw_needed = true;
    }
    if (!(y <= this->window_y + 50 || y >= this->window_y + 170 || x <= this->window_x + 17 ||
        x >= this->window_x + 252 || this->is_dragging))
    {
        int row = (y - window_y - 52) / NPC_LIST_ROW_HEIGHT;
        if (row > 1)
            row = -1;
        if (row != hovered_row)
        {
            hovered_row = row;
            MainForm->redraw_needed = true;
        }
    }
    else
    {
        if (this->hovered_row >= 0)
        {
            this->hovered_row = -1;
            MainForm->redraw_needed = true;
        }
    }
    if (!(x < this->window_x + 91 || x > this->window_x + 181 || y < this->window_y + 159 || y > this->window_y + 185))
    {
        if (!this->close_hovered)
        {
            this->close_hovered = 1;
            MainForm->redraw_needed = true;
        }
    }
    else
    {
        if (close_hovered)
        {
            close_hovered = 0;
            MainForm->redraw_needed = true;
        }
    }
}

// Ends moving the window.
void JukeboxController::MouseUp()
{
    is_dragging = 0;
}
