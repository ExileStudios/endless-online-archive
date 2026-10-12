#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Ctrl_guild.h"
#include "Packetoperations.h"
#include "Ctrl_quests.h"
#pragma package(smart_init)

#define FIRST_LOCAL_POST_ID 50000
#define MSGBOARD_VISIBLE_POSTS 12
#define MSGBOARD_THUMB_TRAVEL 122.0

MsgBoardController::MsgBoardController()
{
    window_x = 77;
    window_y = 16;
    unused_cleared_flag = 0;
    is_composing = false;
    scroll_up_hovered = 0;
    scroll_down_hovered = 0;
    scroll_changed = 0;
    board_id = -1;
    top_line = 0;
    scroll_thumb_offset = 0;
    hovered_row = -1;
    is_visible = 0;
    action_button_hovered = false;
    close_hovered = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    is_dragging = 0;
    next_local_post_id = FIRST_LOCAL_POST_ID;
}

MsgBoardController::~MsgBoardController()
{
}

// Places the subject and body edits over the window, with their limits and colours, and focuses the subject.
void MsgBoardController::CreatePostEdits()
{
    MainForm->edits->AddEdit(20, 109, window_x + 144, window_y + 43, 302, EDIT_LINE_HEIGHT);
    MainForm->edits->AddEdit(21, 109, window_x + 14, window_y + 74, 431, 170);
    MainForm->edits->EnableChangeNotify(20);
    MainForm->edits->EnableChangeNotify(21);
    MainForm->edits->SetText(20, "");
    MainForm->edits->SetTextColor(20, 13158600);
    MainForm->edits->SetCharFilter(20, 1);
    MainForm->edits->SetMaxLength(20, 32);
    MainForm->edits->SetBackgroundVisible(20, 0);
    MainForm->edits->SetBorderVisible(20, 0);
    MainForm->edits->SetText(21, MainForm->strings->GetText(GameText_LoadingMessage));
    MainForm->edits->SetTextColor(21, 13158600);
    MainForm->edits->SetMaxLength(21, 1200);
    MainForm->edits->SetBackgroundVisible(21, 0);
    MainForm->edits->SetBorderVisible(21, 0);
    MainForm->edits->SetMultiLine(21, 1);
    MainForm->edits->FocusEdit(20);
    ResetScroll();
}

// Removes the subject and body edits and gives the focus back to the chat edit.
void MsgBoardController::HidePostEdits()
{
    MainForm->edits->RemoveEdit(20);
    MainForm->edits->RemoveEdit(21);
    MainForm->edits->FocusEdit(EditId_Chat);
}

// Shows the board on its post list, closing the quest window.
void MsgBoardController::Open(int new_board_id)
{
    MainForm->animation->quests->is_visible = 0;
    MainForm->gfx->CreateMsgBoardGraphics();
    MainForm->gfx->LoadMsgBoardGraphics();
    // Without a new board id the last board is kept
    if (new_board_id > 0)
        board_id = new_board_id;
    view_mode = 0;
    top_line = 0;
    scroll_thumb_offset = 0;
    MainForm->edits->SetVisible(13, 0);
    is_visible = 1;
    MainForm->redraw_needed = true;
}

// Hides the board and frees its graphics.
void MsgBoardController::Close()
{
    MainForm->gfx->FreeMsgBoardGraphics();
    is_visible = 0;
    MainForm->redraw_needed = true;
}

// Repeats a held scroll arrow.
DEFINE_TICK(MsgBoardController)

int MsgBoardController::GetScrollThumbOffset()
{
    return scroll_thumb_offset;
}

int MsgBoardController::GetTopLine()
{
    return top_line;
}

// Scrolls back to the top and lets go of the scroll arrows and the thumb.
void MsgBoardController::ResetScroll()
{
    top_line = 0;
    scroll_thumb_offset = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    thumb_grabbed = 0;
}

// Empties the post list.
void MsgBoardController::ClearPosts()
{
    next_local_post_id = FIRST_LOCAL_POST_ID;
    posts.clear();
}

// Stores a post's body from the server, censored, and shows it when that post is open.
void MsgBoardController::SetPostBody(String reply_data)
{
    if (reply_data.Length() > 1)
    {
        String post_id_text = MainForm->net_client->DecodeNumber(reply_data.SubString(1, 2));
        reply_data.Delete(1, 2);
        String body = Client::CensorText(MainForm->net_client, reply_data);
        if (body == "")
            body = "****";
        vector<Msg>::iterator it;
        for (it = posts.begin(); it != posts.end(); it++)
        {
            if (IntToStr(it->post_id) == post_id_text)
            {
                it->body = body;
                break;
            }
        }
        if (view_mode == 1)
        {
            if (IntToStr(current_post_id) == post_id_text)
            {
                current_post_body = body;
                MainForm->edits->SetText(21, body);
                MainForm->edits->SetPendingScroll(21, 10000);
            }
        }
    }
}

// Fills the post list from the server's board reply: each post's id, author and censored subject.
void MsgBoardController::LoadPosts(String reply_data)
{
    if (reply_data.Length() > 1)
    {
        board_id = MainForm->net_client->DecodeNumber(reply_data.SubString(1, 1));
        int post_count = MainForm->net_client->DecodeNumber(reply_data.SubString(2, 1));
        reply_data.Delete(1, 2);
        MainForm->net_client->BeginTokens(reply_data, -1);
        for (int i = 0; i < post_count; i++)
        {
            String post_id_text = MainForm->net_client->NextToken();
            String post_author = MainForm->net_client->NextToken();
            String post_subject = Client::CensorText(MainForm->net_client, MainForm->net_client->NextToken());
            if (post_subject == "")
                post_subject = "****";
            int post_id = MainForm->net_client->DecodeNumber(post_id_text);
            Msg handle(post_id, post_author, post_subject);
            posts.insert(posts.end(), handle);
        }
    }
}

// Sends a new post, cut to the length limits, and goes back to the post list.
void MsgBoardController::SubmitPost(String subject, String body)
{
    if (subject.Length() > 32)
        body = subject.SubString(1, 32);
    if (body.Length() > 1200)
        body = body.SubString(1, 1200);
    next_local_post_id++;
    Msg handle(next_local_post_id, MainForm->players->GetName(MainForm->net_client->GetPlayerId()), subject);
    handle.body = body;
    posts.insert(posts.begin(), handle);
    MainForm->net_client->sender->SendBoardPost(board_id, subject, body);
    HidePostEdits();
    ResetScroll();
    view_mode = 0;
    is_composing = false;
    MainForm->redraw_needed = true;
}

// The number of posts on the board by an author, ignoring case.
int MsgBoardController::CountPostsByAuthor(String author)
{
    int count = 0;
    for (vector<Msg>::iterator it = posts.begin(); it != posts.end(); it++)
    {
        if (LowerCase(it->author) == LowerCase(author))
            count++;
    }
    return count;
}

// Takes the open post off the list, asks the server to delete it, and goes back to the post list.
void MsgBoardController::DeletePost(int post_id)
{
    for (vector<Msg>::iterator it = posts.begin(); it != posts.end(); it++)
    {
        if (IntToStr(it->post_id) == post_id)
        {
            posts.erase(it);
            break;
        }
    }
    MainForm->net_client->sender->SendBoardDeletePost(board_id, current_post_id);
    HidePostEdits();
    ResetScroll();
    view_mode = 0;
    is_composing = false;
    MainForm->redraw_needed = true;
}

// Opens a post in the edits, read-only; its body is asked for when it has not come yet.
void MsgBoardController::ReadPost(int index)
{
    if (posts.size() > (unsigned int)index)
    {
        CreatePostEdits();
        MainForm->edits->SetEditable(POST_SUBJECT_EDIT, 0);
        MainForm->edits->SetEditable(POST_MESSAGE_EDIT, 0);
        is_composing = false;
        current_post_id = posts[index].post_id;
        current_post_author = posts[index].author;
        current_post_subject = posts[index].subject;
        current_post_body = posts[index].body;
        MainForm->edits->SetText(20, current_post_subject);
        if (current_post_body.Length() > 0)
            MainForm->edits->SetText(21, current_post_body);
        else
            MainForm->net_client->sender->RequestBoardPost(board_id, current_post_id);
        MainForm->edits->SetPendingScroll(21, 10000);
        hovered_row = -1;
        is_composing = false;
        view_mode = 1;
    }
}

// A click in the window: on the post list a row opens its post; the scroll bar of the list or the body; any other
// click starts moving the window.
void MsgBoardController::MouseDown(int x, int y, int button)
{
    int __slot1;
    char handled = 0;
    if (button == 0 || button == 1)
    {
        if (window_x <= x && window_x + 282 >= x && y >= window_y && y <= window_y + 290)
        {
            int row = top_line + (y - window_y - 47) / NPC_TEXT_LINE_HEIGHT;
            if (!view_mode)
            {
                if (posts.size() <= (unsigned int)row)
                    row = -1;
                if (row >= 0)
                {
                    handled = 1;
                    ReadPost(row);
                    MainForm->redraw_needed = true;
                    top_line = 0;
                    scroll_thumb_offset = 0;
                }
            }
        }
        if (!handled)
        {
            if (view_mode)
            {
                if (window_x + 449 <= x && window_x + 464 >= x)
                {
                    if (y >= window_y + scroll_thumb_offset + 88 && y <= window_y + scroll_thumb_offset + 103)
                    {
                        handled = 1;
                        thumb_grabbed = 1;
                        thumb_grab_offset = y - scroll_thumb_offset - window_y;
                        DragScrollThumb(y);
                    }
                }
                if (window_x + 449 <= x && window_x + 464 >= x && y >= window_y + 74 && y <= window_y + 89)
                {
                    handled = 1;
                    scroll_up_held = 1;
                    scroll_repeat_delay = 4;
                    ScrollUp();
                }
                if (window_x + 449 <= x && window_x + 464 >= x && y >= window_y + 228 && y <= window_y + 243)
                {
                    handled = 1;
                    scroll_down_held = 1;
                    scroll_repeat_delay = 4;
                    ScrollDown();
                }
            }
            if (!view_mode)
            {
                if (window_x + 449 <= x && window_x + 464 >= x)
                {
                    if (y >= window_y + scroll_thumb_offset + 58 && y <= window_y + scroll_thumb_offset + 73)
                    {
                        handled = 1;
                        thumb_grabbed = 1;
                        thumb_grab_offset = y - scroll_thumb_offset - window_y;
                        DragScrollThumb(y);
                    }
                }
                if (window_x + 449 <= x && window_x + 464 >= x && y >= window_y + 44 && y <= window_y + 59)
                {
                    handled = 1;
                    scroll_up_held = 1;
                    scroll_repeat_delay = 4;
                    ScrollUp();
                }
                if (window_x + 449 <= x && window_x + 464 >= x && y >= window_y + 228 && y <= window_y + 243)
                {
                    handled = 1;
                    scroll_down_held = 1;
                    scroll_repeat_delay = 4;
                    ScrollDown();
                }
            }
        }
    }
    if (!handled)
    {
        if (!this->is_dragging)
        {
            this->is_dragging = 1;
            this->drag_offset_x = x - this->window_x;
            this->drag_offset_y = y - this->window_y;
        }
    }
}

// Moves a dragged window, kept on screen, with the edits; tracks the hovered post and the action and close buttons.
void MsgBoardController::MouseMove(int x, int y)
{
    UpdateScrollHover(x, y);
    if (is_dragging)
    {
        window_x = x - drag_offset_x;
        window_y = y - drag_offset_y;
        if (window_x <= 10)
            window_x = 10;
        if (window_y <= 8)
            window_y = 8;
        if (window_x >= 146)
            window_x = 146;
        if (window_y >= 19)
            window_y = 19;
        MainForm->edits->SetPosition(20, window_x + 144, window_y + 43);
        MainForm->edits->SetPosition(21, window_x + 14, window_y + 74);
        MainForm->redraw_needed = true;
    }
    if (y > window_y + 43 && y < window_y + 243 && x > window_x + 17 && x < window_x + 467 && !is_dragging)
    {
        int row = (y - window_y - 47) / NPC_TEXT_LINE_HEIGHT;
        if (row > 11)
            row = -1;
        if (!view_mode && posts.size() <= (unsigned int)row)
            row = -1;
        if (view_mode)
            row = -1;
        if (row != hovered_row)
        {
            hovered_row = row;
            MainForm->redraw_needed = true;
        }
    }
    else
    {
        if (hovered_row >= 0)
        {
            hovered_row = -1;
            MainForm->redraw_needed = true;
        }
    }
    if (x >= window_x + 284 && x <= window_x + 374 && y >= window_y + 253 && y <= window_y + 279)
    {
        if (!action_button_hovered)
        {
            action_button_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else
    {
        if (action_button_hovered)
        {
            action_button_hovered = false;
            MainForm->redraw_needed = true;
        }
    }
    if (x >= window_x + 380 && x <= window_x + 470 && y >= window_y + 253 && y <= window_y + 279)
    {
        if (!close_hovered)
        {
            close_hovered = 1;
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

// Lights the scroll arrow under the mouse, for the list or the body, and drags a grabbed thumb.
void MsgBoardController::UpdateScrollHover(int x, int y)
{
    if (!view_mode)
    {
        if (x >= this->window_x + 449 && x <= this->window_x + 464)
        {
            if (y >= this->window_y + 58 && y <= this->window_y + 229)
                this->DragScrollThumb(y);
        }
        if (x >= window_x + 449 && x <= window_x + 464 && y >= window_y + 44 && y <= window_y + 59)
        {
            if (!scroll_up_hovered)
            {
                scroll_up_hovered = 1;
                MainForm->redraw_needed = true;
            }
        }
        else if (scroll_up_hovered)
        {
            scroll_up_hovered = 0;
            MainForm->redraw_needed = true;
        }
        if (x >= window_x + 449 && x <= window_x + 464 && y >= window_y + 228 && y <= window_y + 243)
        {
            if (!scroll_down_hovered)
            {
                scroll_down_hovered = 1;
                MainForm->redraw_needed = true;
            }
        }
        else if (scroll_down_hovered)
        {
            scroll_down_hovered = 0;
            MainForm->redraw_needed = true;
        }
    }
    else
    {
        if (x >= window_x + 449 && x <= window_x + 464 && (y >= window_y + 88 && y <= window_y + 229))
            DragScrollThumb(y);
        if (x >= window_x + 449 && x <= window_x + 464 && y >= window_y + 73 && y <= window_y + 89)
        {
            if (!scroll_up_hovered)
            {
                scroll_up_hovered = 1;
                MainForm->redraw_needed = true;
            }
        }
        else if (scroll_up_hovered)
        {
            scroll_up_hovered = 0;
            MainForm->redraw_needed = true;
        }
        if (x >= window_x + 449 && x <= window_x + 464 && y >= window_y + 228 && y <= window_y + 243)
        {
            if (!scroll_down_hovered)
            {
                scroll_down_hovered = 1;
                MainForm->redraw_needed = true;
            }
        }
        else if (scroll_down_hovered)
        {
            scroll_down_hovered = 0;
            MainForm->redraw_needed = true;
        }
    }
}

// Follows a grabbed scroll thumb: it scrolls the open post's body, or picks the top row of the post list.
void MsgBoardController::DragScrollThumb(int y)
{
    int line_count;
    double body_thumb_fraction;
    double body_scaled_line;
    double list_thumb_fraction;
    double list_scaled_row;
    int body_thumb_offset = 0;
    int visible_lines = 0;
    int body_top_line;
    int list_thumb_offset = 0;
    if (view_mode && thumb_grabbed)
    {
        body_thumb_offset = y - thumb_grab_offset - window_y;
        if (body_thumb_offset < 0)
            body_thumb_offset = 0;
        if (body_thumb_offset > 122)
            body_thumb_offset = 122;
        visible_lines = MainForm->edits->GetVisibleLineCount(POST_MESSAGE_EDIT);
        line_count = MainForm->edits->GetLineCount(POST_MESSAGE_EDIT);
        MainForm->edits->GetFirstVisibleLine(POST_MESSAGE_EDIT);
        MainForm->edits->GetScrollOffset(POST_MESSAGE_EDIT);
        if (visible_lines < line_count)
        {
            body_thumb_fraction = body_thumb_offset / 122.0;
            body_scaled_line = ((double)line_count - (double)visible_lines) * body_thumb_fraction;
            body_top_line = body_scaled_line;
            if (body_top_line != top_line)
            {
                top_line = body_top_line;
                MainForm->edits->SetScrollOffset(21, line_count - visible_lines - this->top_line);
            }
        }
        scroll_thumb_offset = body_thumb_offset;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
    if (!view_mode && thumb_grabbed)
    {
        list_thumb_offset = y - thumb_grab_offset - window_y;
        if (list_thumb_offset < 0)
            list_thumb_offset = 0;
        if (list_thumb_offset > 152)
            list_thumb_offset = 152;
        if (posts.size() > MSGBOARD_VISIBLE_POSTS)
        {
            list_thumb_fraction = list_thumb_offset / NPC_THUMB_TRAVEL;
            list_scaled_row = (posts.size() - 12.0) * list_thumb_fraction;
            int list_top_row = list_scaled_row;
            if (list_top_row != top_line)
                top_line = list_top_row;
        }
        scroll_thumb_offset = list_thumb_offset;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the open post's body or the post list up one line and moves the scroll thumb to match.
void MsgBoardController::ScrollUp()
{
    if (view_mode)
    {
        MainForm->edits->ScrollUp(21);
        int visible_lines = MainForm->edits->GetVisibleLineCount(POST_MESSAGE_EDIT);
        int line_count = MainForm->edits->GetLineCount(POST_MESSAGE_EDIT);
        MainForm->edits->GetFirstVisibleLine(POST_MESSAGE_EDIT);
        int scroll_offset = MainForm->edits->GetScrollOffset(POST_MESSAGE_EDIT);
        top_line = 0;
        if (line_count > visible_lines)
        {
            if (scroll_offset > line_count - visible_lines)
                scroll_offset = line_count - visible_lines;
            top_line = line_count - visible_lines - scroll_offset;
            double body_scroll_fraction = (double)top_line / (double)(line_count - visible_lines);
            double body_thumb_pos = MSGBOARD_THUMB_TRAVEL * body_scroll_fraction;
            scroll_thumb_offset = body_thumb_pos;
            scroll_changed = 1;
            MainForm->redraw_needed = true;
        }
    }
    if (!view_mode)
    {
        if ((int)posts.size() > MSGBOARD_VISIBLE_POSTS && top_line > 0)
        {
            top_line--;
            double list_scroll_fraction = (double)top_line / (double)(posts.size() - 12);
            double list_thumb_pos = NPC_THUMB_TRAVEL * list_scroll_fraction;
            scroll_thumb_offset = list_thumb_pos;
            scroll_changed = 1;
            MainForm->redraw_needed = true;
        }
    }
}

// Scrolls the open post's body or the post list down one line and moves the scroll thumb to match.
void MsgBoardController::ScrollDown()
{
    if (view_mode)
    {
        MainForm->edits->ScrollDown(21);
        int visible_lines = MainForm->edits->GetVisibleLineCount(POST_MESSAGE_EDIT);
        int line_count = MainForm->edits->GetLineCount(POST_MESSAGE_EDIT);
        MainForm->edits->GetFirstVisibleLine(POST_MESSAGE_EDIT);
        int scroll_offset = MainForm->edits->GetScrollOffset(POST_MESSAGE_EDIT);
        top_line = 0;
        if (line_count > visible_lines)
        {
            if (scroll_offset > line_count - visible_lines)
                scroll_offset = line_count - visible_lines;
            top_line = line_count - visible_lines - scroll_offset;
            double body_scroll_fraction = (double)top_line / (double)(line_count - visible_lines);
            double body_thumb_pos = MSGBOARD_THUMB_TRAVEL * body_scroll_fraction;
            scroll_thumb_offset = body_thumb_pos;
            scroll_changed = 1;
            MainForm->redraw_needed = true;
        }
    }
    if (!view_mode)
    {
        if ((int)posts.size() - 12 > top_line)
        {
            top_line++;
            double list_scroll_fraction = (double)top_line / (double)(posts.size() - 12);
            double list_thumb_pos = NPC_THUMB_TRAVEL * list_scroll_fraction;
            scroll_thumb_offset = list_thumb_pos;
            scroll_changed = 1;
            MainForm->redraw_needed = true;
        }
    }
}

// Ends moving the window, the held scroll arrows and the grabbed thumb.
void MsgBoardController::MouseUp()
{
    is_dragging = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    thumb_grabbed = 0;
}
