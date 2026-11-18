#include <vcl.h>
#pragma hdrstop

#include "Ctrl_msgboard.h"
#include "Tmainform.h"
#include "Ctrl_guild.h"
#include "Packetoperations.h"
#include "Ctrl_quests.h"

#pragma package(smart_init)

MsgBoardController::MsgBoardController()
{
    window_x = 77;
    window_y = 16;
    unused_cleared_flag = false;
    is_composing = false;
    scroll_up_hovered = false;
    scroll_down_hovered = false;
    scroll_changed = false;
    board_id = -1;
    top_line = 0;
    scroll_thumb_offset = 0;
    hovered_row = -1;
    is_visible = false;
    action_button_hovered = false;
    close_hovered = false;
    scroll_up_held = false;
    scroll_down_held = false;
    is_dragging = false;
    next_local_post_id = FIRST_LOCAL_POST_ID;
}

MsgBoardController::~MsgBoardController()
{
}

// Places the subject and body edits over the window, with their limits and colours, and focuses the subject.
void MsgBoardController::CreatePostEdits()
{
    MainForm->edits->AddEdit(POST_SUBJECT_EDIT, EditGroup_MsgBoard, window_x + 144, window_y + 43, 302,
        EDIT_LINE_HEIGHT);
    MainForm->edits->AddEdit(POST_MESSAGE_EDIT, EditGroup_MsgBoard, window_x + 14, window_y + 74, 431, 170);
    MainForm->edits->EnableChangeNotify(POST_SUBJECT_EDIT);
    MainForm->edits->EnableChangeNotify(POST_MESSAGE_EDIT);
    MainForm->edits->SetText(POST_SUBJECT_EDIT, "");
    MainForm->edits->SetTextColor(POST_SUBJECT_EDIT, 0xC8C8C8);
    MainForm->edits->SetCharFilter(POST_SUBJECT_EDIT, EditCharFilter_Email);
    MainForm->edits->SetMaxLength(POST_SUBJECT_EDIT, 32);
    MainForm->edits->SetBackgroundVisible(POST_SUBJECT_EDIT, false);
    MainForm->edits->SetBorderVisible(POST_SUBJECT_EDIT, false);
    MainForm->edits->SetText(POST_MESSAGE_EDIT, MainForm->strings->GetText(GameText_LoadingMessage));
    MainForm->edits->SetTextColor(POST_MESSAGE_EDIT, 0xC8C8C8);
    MainForm->edits->SetMaxLength(POST_MESSAGE_EDIT, 1200);
    MainForm->edits->SetBackgroundVisible(POST_MESSAGE_EDIT, false);
    MainForm->edits->SetBorderVisible(POST_MESSAGE_EDIT, false);
    MainForm->edits->SetMultiLine(POST_MESSAGE_EDIT, true);
    MainForm->edits->FocusEdit(POST_SUBJECT_EDIT);
    ResetScroll();
}

// Removes the subject and body edits and gives the focus back to the chat edit.
void MsgBoardController::HidePostEdits()
{
    MainForm->edits->RemoveEdit(POST_SUBJECT_EDIT);
    MainForm->edits->RemoveEdit(POST_MESSAGE_EDIT);
    MainForm->edits->FocusEdit(EditId_Chat);
}

// Shows the board on its post list, closing the quest window.
void MsgBoardController::Open(int new_board_id)
{
    MainForm->animation->quests->is_visible = false;
    MainForm->gfx->CreateMsgBoardGraphics();
    MainForm->gfx->LoadMsgBoardGraphics();
    // Without a new board id the last board is kept
    if (new_board_id > 0)
        board_id = new_board_id;
    view_mode = 0;
    top_line = 0;
    scroll_thumb_offset = 0;
    MainForm->edits->SetVisible(EditId_Amount, false);
    is_visible = true;
    MainForm->redraw_needed = true;
}

// Goes back to the list from any other view; from the list, closes the board and frees its pictures.
void MsgBoardController::Close()
{
    if (view_mode > 0)
    {
        ResetScroll();
        view_mode = 0;
        HidePostEdits();
    }
    else
    {
        MainForm->gfx->FreeMsgBoardGraphics();
        is_visible = false;
    }
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
    scroll_up_held = false;
    scroll_down_held = false;
    thumb_grabbed = false;
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
    vector<Msg>::iterator it;
    if (reply_data.Length() > 1)
    {
        String post_id_text((unsigned int)MainForm->net_client->DecodeNumber(reply_data.SubString(1, 2)));
        reply_data.Delete(1, 2);
        String body = MainForm->net_client->CensorText(reply_data);
        if (body == "")
            body = "****";
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
                MainForm->edits->SetText(POST_MESSAGE_EDIT, body);
                MainForm->edits->SetPendingScroll(POST_MESSAGE_EDIT, 10000);
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
            String post_subject = MainForm->net_client->CensorText(MainForm->net_client->NextToken());
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
    if (posts.size() > index)
    {
        CreatePostEdits();
        MainForm->edits->SetEditable(POST_SUBJECT_EDIT, false);
        MainForm->edits->SetEditable(POST_MESSAGE_EDIT, false);
        is_composing = false;
        current_post_id = posts[index].post_id;
        current_post_author = posts[index].author;
        current_post_subject = posts[index].subject;
        current_post_body = posts[index].body;
        MainForm->edits->SetText(POST_SUBJECT_EDIT, current_post_subject);
        if (current_post_body.Length() > 0)
            MainForm->edits->SetText(POST_MESSAGE_EDIT, current_post_body);
        else
            MainForm->net_client->sender->RequestBoardPost(board_id, current_post_id);
        MainForm->edits->SetPendingScroll(POST_MESSAGE_EDIT, 10000);
        hovered_row = -1;
        is_composing = false;
        view_mode = 1;
    }
}

// A click in the window: on the post list a row opens its post; the scroll bar of the list or the body; any other
// click starts moving the window.
void MsgBoardController::MouseDown(int x, int y, int button)
{
    char handled = false;
    if (button == 0 || button == 1)
    {
        if (window_x <= x && window_x + 282 >= x && y >= window_y && y <= window_y + 290)
        {
            int row = top_line + (y - window_y - 47) / NPC_TEXT_LINE_HEIGHT;
            // In the list view a click on a post opens it.
            if (!view_mode)
            {
                if (posts.size() <= row)
                    row = -1;
                if (row >= 0)
                {
                    handled = true;
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
                        handled = true;
                        thumb_grabbed = true;
                        thumb_grab_offset = y - scroll_thumb_offset - window_y;
                        DragScrollThumb(y);
                    }
                }
                if (window_x + 449 <= x && window_x + 464 >= x && y >= window_y + 74 && y <= window_y + 89)
                {
                    handled = true;
                    scroll_up_held = true;
                    scroll_repeat_delay = 4;
                    ScrollUp();
                }
                if (window_x + 449 <= x && window_x + 464 >= x && y >= window_y + 228 && y <= window_y + 243)
                {
                    handled = true;
                    scroll_down_held = true;
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
                        handled = true;
                        thumb_grabbed = true;
                        thumb_grab_offset = y - scroll_thumb_offset - window_y;
                        DragScrollThumb(y);
                    }
                }
                if (window_x + 449 <= x && window_x + 464 >= x && y >= window_y + 44 && y <= window_y + 59)
                {
                    handled = true;
                    scroll_up_held = true;
                    scroll_repeat_delay = 4;
                    ScrollUp();
                }
                if (window_x + 449 <= x && window_x + 464 >= x && y >= window_y + 228 && y <= window_y + 243)
                {
                    handled = true;
                    scroll_down_held = true;
                    scroll_repeat_delay = 4;
                    ScrollDown();
                }
            }
        }
    }
    if (!handled)
    {
        if (!is_dragging)
        {
            is_dragging = true;
            drag_offset_x = x - window_x;
            drag_offset_y = y - window_y;
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
        MainForm->edits->SetPosition(POST_SUBJECT_EDIT, window_x + 144, window_y + 43);
        MainForm->edits->SetPosition(POST_MESSAGE_EDIT, window_x + 14, window_y + 74);
        MainForm->redraw_needed = true;
    }
    if (y > window_y + 43 && y < window_y + 243 && x > window_x + 17 && x < window_x + 467 && !is_dragging)
    {
        int row = (y - window_y - 47) / NPC_TEXT_LINE_HEIGHT;
        if (row > 11)
            row = -1;
        if (!view_mode)
        {
            if (posts.size() <= row)
                row = -1;
        }
        if (view_mode)
            row = -1;
        if (row != hovered_row)
        {
            hovered_row = row;
            MainForm->redraw_needed = true;
        }
    }
    else if (hovered_row >= 0)
    {
        hovered_row = -1;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 284 && x <= window_x + 374 && y >= window_y + 253 && y <= window_y + 279)
    {
        if (!action_button_hovered)
        {
            action_button_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else if (action_button_hovered)
    {
        action_button_hovered = false;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 380 && x <= window_x + 470 && y >= window_y + 253 && y <= window_y + 279)
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

// Lights the scroll arrow under the mouse, for the list or the body, and drags a grabbed thumb.
void MsgBoardController::UpdateScrollHover(int x, int y)
{
    if (!view_mode)
    {
        if (x >= window_x + 449 && x <= window_x + 464)
        {
            if (y >= window_y + 58 && y <= window_y + 229)
                DragScrollThumb(y);
        }
        if (x >= window_x + 449 && x <= window_x + 464 && y >= window_y + 44 && y <= window_y + 59)
        {
            if (!scroll_up_hovered)
            {
                scroll_up_hovered = true;
                MainForm->redraw_needed = true;
            }
        }
        else if (scroll_up_hovered)
        {
            scroll_up_hovered = false;
            MainForm->redraw_needed = true;
        }
        if (x >= window_x + 449 && x <= window_x + 464 && y >= window_y + 228 && y <= window_y + 243)
        {
            if (!scroll_down_hovered)
            {
                scroll_down_hovered = true;
                MainForm->redraw_needed = true;
            }
        }
        else if (scroll_down_hovered)
        {
            scroll_down_hovered = false;
            MainForm->redraw_needed = true;
        }
    }
    else
    {
        if (x >= window_x + 449 && x <= window_x + 464)
        {
            if (y >= window_y + 88 && y <= window_y + 229)
                DragScrollThumb(y);
        }
        if (x >= window_x + 449 && x <= window_x + 464 && y >= window_y + 73 && y <= window_y + 89)
        {
            if (!scroll_up_hovered)
            {
                scroll_up_hovered = true;
                MainForm->redraw_needed = true;
            }
        }
        else if (scroll_up_hovered)
        {
            scroll_up_hovered = false;
            MainForm->redraw_needed = true;
        }
        if (x >= window_x + 449 && x <= window_x + 464 && y >= window_y + 228 && y <= window_y + 243)
        {
            if (!scroll_down_hovered)
            {
                scroll_down_hovered = true;
                MainForm->redraw_needed = true;
            }
        }
        else if (scroll_down_hovered)
        {
            scroll_down_hovered = false;
            MainForm->redraw_needed = true;
        }
    }
}

// Follows a grabbed scroll thumb: it scrolls the open post's body, or picks the top row of the post list.
void MsgBoardController::DragScrollThumb(int y)
{
    int list_thumb_offset;
    int body_top_line;
    int list_top_row;
    if (view_mode && thumb_grabbed)
    {
        int body_thumb_offset = y - thumb_grab_offset - window_y;
        if (body_thumb_offset < 0)
            body_thumb_offset = 0;
        if (body_thumb_offset > 122)
            body_thumb_offset = 122;
        int visible_lines = MainForm->edits->GetVisibleLineCount(POST_MESSAGE_EDIT);
        int line_count = MainForm->edits->GetLineCount(POST_MESSAGE_EDIT);
        MainForm->edits->GetFirstVisibleLine(POST_MESSAGE_EDIT);
        MainForm->edits->GetScrollOffset(POST_MESSAGE_EDIT);
        if (visible_lines < line_count)
        {
            double body_thumb_fraction = body_thumb_offset / 122.0;
            double body_scaled_line = ((double)line_count - visible_lines) * body_thumb_fraction;
            body_top_line = body_scaled_line;
            if (body_top_line != top_line)
            {
                top_line = body_top_line;
                MainForm->edits->SetScrollOffset(POST_MESSAGE_EDIT, line_count - visible_lines - top_line);
            }
        }
        scroll_thumb_offset = body_thumb_offset;
        scroll_changed = true;
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
            double list_thumb_fraction = list_thumb_offset / NPC_THUMB_TRAVEL;
            double list_scaled_row = (posts.size() - 12.0) * list_thumb_fraction;
            list_top_row = list_scaled_row;
            if (list_top_row != top_line)
                top_line = list_top_row;
        }
        scroll_thumb_offset = list_thumb_offset;
        scroll_changed = true;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the open post's body or the post list up one line and moves the scroll thumb to match.
void MsgBoardController::ScrollUp()
{
    if (view_mode)
    {
        MainForm->edits->ScrollUp(POST_MESSAGE_EDIT);
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
            double body_scroll_fraction = (double)top_line / (line_count - visible_lines);
            double body_thumb_pos = MSGBOARD_THUMB_TRAVEL * body_scroll_fraction;
            scroll_thumb_offset = body_thumb_pos;
            scroll_changed = true;
            MainForm->redraw_needed = true;
        }
    }
    if (!view_mode)
    {
        if ((int)posts.size() > MSGBOARD_VISIBLE_POSTS && top_line > 0)
        {
            top_line--;
            double list_scroll_fraction = (double)top_line / (posts.size() - 12);
            double list_thumb_pos = NPC_THUMB_TRAVEL * list_scroll_fraction;
            scroll_thumb_offset = list_thumb_pos;
            scroll_changed = true;
            MainForm->redraw_needed = true;
        }
    }
}

// Scrolls the open post's body or the post list down one line and moves the scroll thumb to match.
void MsgBoardController::ScrollDown()
{
    if (view_mode)
    {
        MainForm->edits->ScrollDown(POST_MESSAGE_EDIT);
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
            double body_scroll_fraction = (double)top_line / (line_count - visible_lines);
            double body_thumb_pos = MSGBOARD_THUMB_TRAVEL * body_scroll_fraction;
            scroll_thumb_offset = body_thumb_pos;
            scroll_changed = true;
            MainForm->redraw_needed = true;
        }
    }
    if (!view_mode)
    {
        if ((int)posts.size() - 12 > top_line)
        {
            top_line++;
            double list_scroll_fraction = (double)top_line / (posts.size() - 12);
            double list_thumb_pos = NPC_THUMB_TRAVEL * list_scroll_fraction;
            scroll_thumb_offset = list_thumb_pos;
            scroll_changed = true;
            MainForm->redraw_needed = true;
        }
    }
}

// Ends moving the window, the held scroll arrows and the grabbed thumb.
void MsgBoardController::MouseUp()
{
    is_dragging = false;
    scroll_up_held = false;
    scroll_down_held = false;
    thumb_grabbed = false;
}
