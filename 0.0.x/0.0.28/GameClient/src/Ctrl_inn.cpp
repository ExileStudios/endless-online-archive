#include <vcl.h>
#pragma hdrstop

#include "Ctrl_inn.h"
#include "Tmainform.h"
#include "Ctrl_guild.h"
#include "Ctrl_game.h"
#include "Ctrl_law.h"
#include "Ctrl_memo.h"
#include "Packetoperations.h"
#pragma package(smart_init)

InnController::InnController()
{
    lines = new TStringList();
    unused_lines = new TStringList();
    window_x = 49;
    window_y = 15;
    unused_flag = 1;
    unused_value = 0;
    scroll_up_hovered = 0;
    scroll_down_hovered = 0;
    scroll_changed = 0;
    npc_session_id = -1;
    top_line = 0;
    scroll_thumb_offset = 0;
    hovered_row = -1;
    is_visible = 0;
    close_hovered = 0;
    back_hovered = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    is_dragging = 0;
}

InnController::~InnController()
{
}

DEFINE_ADD_TEXT(InnController)

// Shows the innkeeper's window on its menu page, noting whether the character is a citizen here, and closes the
// other npc windows.
void InnController::Open(int session_id, int citizen_flag)
{
    MainForm->animation->shop->is_visible = 0;
    MainForm->animation->locker->is_visible = 0;
    MainForm->animation->learn->is_visible = 0;
    MainForm->animation->guild->is_visible = 0;
    MainForm->animation->memo->is_visible = 0;
    MainForm->animation->law->is_visible = 0;
    MainForm->gfx->CreateShopGraphics();
    MainForm->gfx->LoadShopGraphics();
    lines->Clear();
    top_line = 0;
    scroll_thumb_offset = 0;
    question_index = 0;
    sleep_request_cooldown = 0;
    npc_session_id = session_id;
    is_citizen = citizen_flag;
    page = 0;
    reset_value = 0;
    link_line_index = 0;
    is_visible = 1;
    MainForm->redraw_needed = true;
}

// Hides the window and frees its graphics.
void InnController::Close()
{
    is_visible = 0;
    npc_session_id = -1;
    MainForm->gfx->FreeShopGraphics();
    MainForm->redraw_needed = true;
}

// Counts down the wait between sleep requests and repeats a held scroll arrow.
void InnController::Tick()
{
    if (sleep_request_cooldown > 0)
        sleep_request_cooldown--;
    if (scroll_repeat_delay > 0)
        scroll_repeat_delay--;
    else
    {
        if (scroll_up_held)
            ScrollUp();
        if (scroll_down_held)
            ScrollDown();
    }
}

int InnController::GetScrollThumbOffset()
{
    return scroll_thumb_offset;
}

int InnController::GetTopLine()
{
    return top_line;
}

// A click in the window. On the menu: open the citizenship page or ask to sleep. On a text page the link line
// starts the first question, or gives citizenship up. Otherwise the scroll bar and moving the window.
void InnController::MouseDown(int x, int y, int button)
{
    bool flag = false;
    if (button == 0 || button == 1)
    {
        if (y > window_y + 43 && y < window_y + 243 && x > window_x + 17 && x < window_x + 252)
        {
            is_dragging = 0;
            int row = top_line + (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
            if (page >= 6)
            {
                row = top_line + (y - window_y - 47) / NPC_TEXT_LINE_HEIGHT;
                if (row == link_line_index)
                {
                    MainForm->sfx->PlaySfx(4);
                    if (page == 6)
                    {
                        // A citizen gets a message instead of the questions
                        if (is_citizen)
                            MainForm->animation->game->ShowMessageBox(0, ConfirmAction_None,
                                MainForm->strings->GetText(GameText_RegistrationService2),
                                MainForm->strings->GetText(GameText_YouAreAlreadyCitizen));
                        else
                        {
                            question_index = 0;
                            String question_prompt = IntToStr(1) + ". " + questions[0];
                            MainForm->animation->game->OpenInputBox(InputBoxKind_InnAnswer, question_prompt, 2, 12);
                        }
                    }
                    if (page == 7)
                    {
                        MainForm->net_client->sender->SendGiveUpCitizenship(npc_session_id);
                        page = 0;
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (page == 1)
            {
                if (!row)
                {
                    ClearText();
                    AddText(MainForm->strings->GetText(GameText_SignUp), true);
                    AddText(MainForm->strings->GetText(GameText_YouAreAboutToRequestTo), true);
                    AddText(MainForm->strings->GetText(GameText_IfYouAreAble), true);
                    AddText(MainForm->strings->GetText(GameText_ClickHereToTake), true);
                    page = 6;
                    MainForm->redraw_needed = true;
                }
                if (row == 1)
                {
                    ClearText();
                    AddText(MainForm->strings->GetText(GameText_Unsubscribe), true);
                    AddText(MainForm->strings->GetText(GameText_YouAreAboutToGive), true);
                    AddText(MainForm->strings->GetText(GameText_ClickHereToUnsubscribe), true);
                    page = 7;
                    MainForm->redraw_needed = true;
                }
            }
            if (!page)
            {
                if (!row)
                {
                    page = 1;
                    MainForm->redraw_needed = true;
                }
                // Sleep requests are spaced out
                if (row == 1 && sleep_request_cooldown <= 0)
                {
                    sleep_request_cooldown = 20;
                    MainForm->net_client->sender->RequestInnSleep(npc_session_id);
                    MainForm->redraw_needed = true;
                }
            }
        }
    }
    if (!flag)
    {
        if (x >= window_x + 252 && x <= window_x + 267)
        {
            if (y >= window_y + scroll_thumb_offset + 58 && y <= window_y + scroll_thumb_offset + 73)
            {
                thumb_grabbed = 1;
                thumb_grab_offset = y - scroll_thumb_offset - window_y;
                DragScrollThumb(y);
            }
        }
        if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 44 && y <= window_y + 59)
        {
            scroll_up_held = 1;
            scroll_repeat_delay = 4;
            ScrollUp();
        }
        if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 228 && y <= window_y + 243)
        {
            scroll_down_held = 1;
            scroll_repeat_delay = 4;
            ScrollDown();
        }
        if (x >= window_x && x <= window_x + 284 && y >= window_y && y <= window_y + 290 && !thumb_grabbed)
        {
            is_dragging = 1;
            drag_offset_x = x - window_x;
            drag_offset_y = y - window_y;
        }
    }
}

// Moves a dragged window, kept on screen, and tracks the hovered row and the close and back buttons.
void InnController::MouseMove(int x, int y)
{
    int row;
    UpdateScrollHover(x, y);
    if (is_dragging)
    {
        window_x = x - drag_offset_x;
        window_y = y - drag_offset_y;
        if (window_x <= 10)
            window_x = 10;
        if (window_y <= 8)
            window_y = 8;
        if (window_x >= 346)
            window_x = 346;
        if (window_y >= 19)
            window_y = 19;
        MainForm->redraw_needed = true;
    }
    if (!(y <= this->window_y + 43 || y >= this->window_y + 243 || x <= this->window_x + 17 ||
        x >= this->window_x + 252 || this->is_dragging))
    {
        if (!page)
        {
            row = (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
            if (row > 1)
                row = -1;
        }
        if (page == 1)
        {
            row = (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
            if (row > 1)
                row = -1;
        }
        if (page >= 6)
        {
            row = (y - window_y - 47) / NPC_TEXT_LINE_HEIGHT;
            if (row > 11)
                row = -1;
            if (lines->Count <= row)
                row = -1;
        }
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
    if (!page)
    {
        if (!(x < this->window_x + 92 || x > this->window_x + 182 || y < this->window_y + 253 ||
            y > this->window_y + 279))
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
    else
    {
        if (!(x < this->window_x + 144 || x > this->window_x + 234 || y < this->window_y + 253 ||
            y > this->window_y + 279))
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
        if (!(x < this->window_x + 48 || x > this->window_x + 138 || y < this->window_y + 253 ||
            y > this->window_y + 279))
        {
            if (!this->back_hovered)
            {
                this->back_hovered = 1;
                MainForm->redraw_needed = true;
            }
        }
        else
        {
            if (back_hovered)
            {
                back_hovered = 0;
                MainForm->redraw_needed = true;
            }
        }
    }
}

// Lights the scroll arrow under the mouse and drags a grabbed thumb.
DEFINE_UPDATE_SCROLL_HOVER(InnController)

DEFINE_DRAG_SCROLL_THUMB(InnController)

DEFINE_SCROLL_UP(InnController)

DEFINE_SCROLL_DOWN(InnController)

// Ends moving the window, the held scroll arrows and the grabbed thumb.
void InnController::MouseUp()
{
    is_dragging = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    thumb_grabbed = 0;
}

DEFINE_CLEAR_TEXT(InnController)
