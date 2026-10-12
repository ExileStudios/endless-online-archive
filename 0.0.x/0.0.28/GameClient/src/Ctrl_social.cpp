#include <vcl.h>
#pragma hdrstop

#include "Ctrl_social.h"
#include "Tmainform.h"
#include "Ctrl_guild.h"
#include "Ctrl_game.h"
#pragma package(smart_init)
#define SHORT_NAME_LENGTH 3

SocialController::SocialController()
{
    int ctor_spare_int;
    int ctor_spare_value;
    int ctor_spare_word;
    window_x = 333;
    window_y = 13;
    unread_flag_at_31 = true;
    unread_int_at_48 = 0;
    scroll_up_hover = 0;
    scroll_down_hover = 0;
    scroll_moved_flag = false;
    scroll_offset = 0;
    scrollbar_position = 0;
    hover_row = -1;
    is_open = 0;
    close_button_hover = false;
    add_button_hover = false;
    scrolling_up = 0;
    scrolling_down = 0;
    is_dragging_window = 0;
    ignored_names = new TStringList();
    friend_names = new TStringList();
    online_names = new TStringList();
}

// Saves both lists when the client closes.
SocialController::~SocialController()
{
    SaveIgnoreList();
    SaveFriendList();
}

// Loads the window graphics and opens it on the friend list, at the top.
void SocialController::OpenWindow()
{
    MainForm->gfx->CreateSocialGraphics();
    MainForm->gfx->LoadSocialGraphics();
    list_kind = 0;
    scroll_offset = 0;
    scrollbar_position = 0;
    MainForm->edits->SetVisible(13, 0);
    MainForm->edits->SetEditable(POST_SUBJECT_EDIT, 0);
    MainForm->edits->SetEditable(POST_MESSAGE_EDIT, 0);
    is_open = 1;
    MainForm->redraw_needed = true;
}

// Closes the window and frees its graphics.
void SocialController::CloseWindow()
{
    is_open = 0;
    if (!MainForm->animation->game->message_visible)
    {
        MainForm->edits->SetEditable(POST_SUBJECT_EDIT, 1);
        MainForm->edits->SetEditable(POST_MESSAGE_EDIT, 1);
    }
    MainForm->gfx->FreeSocialGraphics();
    MainForm->redraw_needed = true;
}

// Reads config/ignore.ini, keeping names of 4 to 12 letters and dropping lines that start with "#".
void SocialController::LoadIgnoreList()
{
    ignored_names->Clear();
    try
    {
        if (!access("./config/ignore.ini", 0))
            ignored_names->LoadFromFile("./config/ignore.ini");
        for (int i = ignored_names->Count - 1; i >= 0; i--)
        {
            if (ignored_names->Strings[i].Length() < 13 && ignored_names->Strings[i].Length() > SHORT_NAME_LENGTH)
            {
                if (ignored_names->Strings[i][1] == 35)
                    ignored_names->Delete(i);
            }
            else
                ignored_names->Delete(i);
        }
    }
    catch (...)
    {
        ignored_names->Clear();
    }
}

// Reads config/friends.ini, keeping names of 4 to 12 letters and dropping lines that start with "#".
void SocialController::LoadFriendList()
{
    friend_names->Clear();
    try
    {
        if (!access("./config/friends.ini", 0))
            friend_names->LoadFromFile("./config/friends.ini");
        for (int i = friend_names->Count - 1; i >= 0; i--)
        {
            if (friend_names->Strings[i].Length() < 13 && friend_names->Strings[i].Length() > SHORT_NAME_LENGTH)
            {
                if (friend_names->Strings[i][1] == 35)
                    friend_names->Delete(i);
            }
            else
                friend_names->Delete(i);
        }
    }
    catch (...)
    {
        friend_names->Clear();
    }
}

// Takes the name off the friend list, then adds it to the ignore list, or takes it off when it is there.
void SocialController::ToggleIgnore(String name)
{
    char was_listed = 0;
    int other_index = friend_names->Count - 1;
    while (other_index >= 0)
    {
        if (LowerCase(friend_names->Strings[other_index]) == LowerCase(name))
        {
            friend_names->Delete(other_index);
            break;
        }
        other_index--;
    }
    int index = this->ignored_names->Count - 1;
    while (index >= 0)
    {
        if (LowerCase(ignored_names->Strings[index]) == LowerCase(name))
        {
            ignored_names->Delete(index);
            was_listed = 1;
            break;
        }
        index--;
    }
    if (!was_listed)
    {
        ignored_names->Add(name);
        ignored_names->Sort();
    }
}

// Takes the name off the friend list and adds it to the ignore list unless it is there already.
void SocialController::AddIgnore(String name)
{
    char is_listed = 0;
    int other_index = friend_names->Count - 1;
    while (other_index >= 0)
    {
        if (LowerCase(friend_names->Strings[other_index]) == LowerCase(name))
        {
            friend_names->Delete(other_index);
            break;
        }
        other_index--;
    }
    int index = this->ignored_names->Count - 1;
    while (index >= 0)
    {
        if (LowerCase(ignored_names->Strings[index]) == LowerCase(name))
        {
            is_listed = 1;
            break;
        }
        index--;
    }
    if (!is_listed)
    {
        ignored_names->Add(name);
        ignored_names->Sort();
    }
}

// Takes the name off the ignore list, then adds it to the friend list, or takes it off when it is there.
void SocialController::ToggleFriend(String name)
{
    char was_listed = 0;
    int other_index = ignored_names->Count - 1;
    while (other_index >= 0)
    {
        if (LowerCase(ignored_names->Strings[other_index]) == LowerCase(name))
        {
            ignored_names->Delete(other_index);
            break;
        }
        other_index--;
    }
    int index = this->friend_names->Count - 1;
    while (index >= 0)
    {
        if (LowerCase(friend_names->Strings[index]) == LowerCase(name))
        {
            friend_names->Delete(index);
            was_listed = 1;
            break;
        }
        index--;
    }
    if (!was_listed)
    {
        friend_names->Add(name);
        friend_names->Sort();
    }
}

// Takes the name off the ignore list and adds it to the friend list unless it is there already.
void SocialController::AddFriend(String name)
{
    char is_listed = 0;
    int other_index = ignored_names->Count - 1;
    while (other_index >= 0)
    {
        if (LowerCase(ignored_names->Strings[other_index]) == LowerCase(name))
        {
            ignored_names->Delete(other_index);
            break;
        }
        other_index--;
    }
    int index = this->friend_names->Count - 1;
    while (index >= 0)
    {
        if (LowerCase(friend_names->Strings[index]) == LowerCase(name))
        {
            is_listed = 1;
            break;
        }
        index--;
    }
    if (!is_listed)
    {
        friend_names->Add(name);
        friend_names->Sort();
    }
}

void SocialController::ClearOnlineNames()
{
    online_names->Clear();
}

// Adds one name from the server's list of players online.
void SocialController::AddOnlineName(String name)
{
    char is_listed = 0;
    if (friend_names->Count > 100)
    {
        int index = friend_names->Count - 1;
        while (index >= 0)
        {
            if (friend_names->Strings[index] == name)
            {
                is_listed = 1;
                break;
            }
            index--;
        }
    }
    if (!is_listed)
        online_names->Add(name);
}

// True when the name, in any case, is in the server's list of players online.
bool SocialController::IsOnline(String name)
{
    bool is_online = 0;
    for (int index = online_names->Count - 1; index >= 0; index--)
    {
        if (LowerCase(online_names->Strings[index]) == LowerCase(name))
        {
            is_online = 1;
            break;
        }
    }
    return is_online;
}

// Writes config/ignore.ini: two comment lines, then the names of 4 to 12 letters in order.
void SocialController::SaveIgnoreList()
{
    for (int i = ignored_names->Count - 1; i >= 0; i--)
    {
        if (ignored_names->Strings[i].Length() < 13 && ignored_names->Strings[i].Length() > SHORT_NAME_LENGTH)
        {
            if (ignored_names->Strings[i][1] == 35)
                ignored_names->Delete(i);
        }
        else
            ignored_names->Delete(i);
    }
    ignored_names->Sort();
    // Reads "# Endless Online 0.28 [ ignore list ]".
    std::string first_header_line = "#]  Etnsdllie seso rOnngiil n[e  802.";
    // Reads "# List of ignored characters, use a new line for each name".
    std::string second_header_line = "#e mLains th cfaoe  ifgonrr oeendl ic hwaerna cat eerssu, ";
    ignored_names->Insert(0, "");
    ignored_names->Insert(0, UnscrambleString(this, 3, second_header_line.begin(), second_header_line.end()));
    ignored_names->Insert(0, "");
    ignored_names->Insert(0, UnscrambleString(this, 3, first_header_line.begin(), first_header_line.end()));
    ignored_names->SaveToFile("./config/ignore.ini");
}

// Writes config/friends.ini: two comment lines, then the names of 4 to 12 letters in order.
void SocialController::SaveFriendList()
{
    for (int i = friend_names->Count - 1; i >= 0; i--)
    {
        if (friend_names->Strings[i].Length() < 13 && friend_names->Strings[i].Length() > SHORT_NAME_LENGTH)
        {
            if (friend_names->Strings[i][1] == 35)
                friend_names->Delete(i);
        }
        else
            friend_names->Delete(i);
    }
    friend_names->Sort();
    // Reads "# Endless Online 0.28 [ friend list ]".
    std::string first_header_line = "#]  Etnsdllie sdsn eOfnriil n[e  802.";
    // Reads "# List of friend characters, use a new line for each name".
    std::string second_header_line = "#e mLains th cfaoe  ifrofre nedn lcih awreanc tae ress,u ";
    friend_names->Insert(0, "");
    friend_names->Insert(0, UnscrambleString(this, 3, second_header_line.begin(), second_header_line.end()));
    friend_names->Insert(0, "");
    friend_names->Insert(0, UnscrambleString(this, 3, first_header_line.begin(), first_header_line.end()));
    friend_names->SaveToFile("./config/friends.ini");
}

// True when the name, in any case, is on the ignore list.
bool SocialController::IsIgnored(String name)
{
    bool is_ignored = 0;
    for (int i = 0; i < ignored_names->Count; i++)
    {
        if (LowerCase(name) == LowerCase(ignored_names->Strings[i]))
        {
            is_ignored = 1;
            break;
        }
    }
    return is_ignored;
}

// True when the name, in any case, is on the friend list.
bool SocialController::IsFriend(String name)
{
    bool is_friend = 0;
    for (int i = 0; i < friend_names->Count; i++)
    {
        if (LowerCase(name) == LowerCase(friend_names->Strings[i]))
        {
            is_friend = 1;
            break;
        }
    }
    return is_friend;
}

// Turns a stored text back into plain text in two steps:
// 1 = every other character from the first, in order, then the rest from last to first
// 2 = each run of characters whose code divides by divisor is turned around
String SocialController::UnscrambleString(SocialController *self, int divisor, unsigned char *data_begin,
    unsigned char *data_end)
{
    int divisor_copy;
    char to_first_list;
    String result = "";
    divisor_copy = divisor;
    try
    {
        std::stack<char > char_stack;
        std::queue<char > first_deque_base;
        to_first_list = 1;
        while (data_begin != data_end)
        {
            if (to_first_list)
            {
                first_deque_base.push(*(char *)data_begin);
                to_first_list = 0;
            }
            else
            {
                char_stack.push(*(char *)data_begin);
                to_first_list = 1;
            }
            data_begin++;
        }
        while (!char_stack.empty())
        {
            first_deque_base.push(char_stack.top());
            char_stack.pop();
        }
        char_stack.empty();
        while (!first_deque_base.empty())
        {
            char current_char = first_deque_base.front();
            first_deque_base.pop();
            unsigned char current_char_copy = current_char;
            int current_char_int = current_char_copy;
            if (!(current_char_int % divisor_copy))
                char_stack.push(current_char);
            else
            {
                while (!char_stack.empty())
                {
                    result.Insert(char_stack.top(), result.Length() + 1);
                    char_stack.pop();
                }
                result.Insert(current_char, result.Length() + 1);
            }
        }
        while (!char_stack.empty())
        {
            result.Insert(char_stack.top(), result.Length() + 1);
            char_stack.pop();
        }
    }
    catch (...)
    {
        result = "";
    }
    return result;
}

void SocialController::OnTimer()
{
    if (scroll_repeat_delay > 0)
        scroll_repeat_delay--;
    else
    {
        if (scrolling_up)
            ScrollUp();
        if (scrolling_down)
            ScrollDown();
    }
}

int SocialController::GetScrollbarPosition()
{
    return scrollbar_position;
}

int SocialController::GetScrollOffset()
{
    return scroll_offset;
}

// A left click on a friend puts "!name " in the chat box; a right click takes a name off its list.
// Off the list, starts a thumb drag, an arrow scroll or a window drag.
void SocialController::OnMouseClick(int mouse_x, int mouse_y, int button)
{
    char handled = 0;
    if (button == 0 || button == 1)
    {
        if (mouse_y > window_y + 43 && mouse_y < window_y + 243 && window_x + 17 < mouse_x && window_x + 252 > mouse_x)
        {
            is_dragging_window = 0;
            int row = scroll_offset + (mouse_y - window_y - 47) / NPC_TEXT_LINE_HEIGHT;
            if (!list_kind)
            {
                if (friend_names->Count <= row)
                    row = -1;
                if (row >= 0 && button == 1)
                {
                    ToggleFriend(friend_names->Strings[row]);
                    MainForm->redraw_needed = true;
                    handled = 1;
                    scroll_offset = 0;
                    scrollbar_position = 0;
                }
                if (row >= 0 && !button)
                {
                    if (friend_names->Strings[row] != "")
                    {
                        MainForm->edits->SetText(EditId_Chat, "!" + friend_names->Strings[row] + " ");
                        MainForm->edits->HasDirtyEdit();
                        handled = 1;
                    }
                }
            }
            if (list_kind == 1)
            {
                if (ignored_names->Count <= row)
                    row = -1;
                if (row >= 0 && button == 1)
                {
                    ToggleIgnore(ignored_names->Strings[row]);
                    MainForm->redraw_needed = true;
                    handled = 1;
                    scroll_offset = 0;
                    scrollbar_position = 0;
                }
            }
        }
    }
    if (!handled)
    {
        if (window_x + 252 <= mouse_x && window_x + 267 >= mouse_x)
        {
            if (mouse_y >= window_y + scrollbar_position + 58 && mouse_y <= window_y + scrollbar_position + 73)
            {
                is_dragging_thumb = 1;
                thumb_grab_offset = mouse_y - scrollbar_position - window_y;
                DragScrollbar(mouse_y);
            }
        }
        if (window_x + 252 <= mouse_x && window_x + 267 >= mouse_x && mouse_y >= window_y + 44 &&
            mouse_y <= window_y + 59)
        {
            scrolling_up = 1;
            scroll_repeat_delay = 4;
            ScrollUp();
        }
        if (window_x + 252 <= mouse_x && window_x + 267 >= mouse_x && mouse_y >= window_y + 228 &&
            mouse_y <= window_y + 243)
        {
            scrolling_down = 1;
            scroll_repeat_delay = 4;
            ScrollDown();
        }
        if (window_x <= mouse_x && window_x + 284 >= mouse_x && mouse_y >= window_y && mouse_y <= window_y + 290 &&
            !is_dragging_thumb)
        {
            is_dragging_window = 1;
            drag_offset_x = mouse_x - window_x;
            drag_offset_y = mouse_y - window_y;
        }
    }
}

// Moves the window while it is dragged and lights the row and the button under the mouse.
void SocialController::OnMouseMove(int mouse_x, int mouse_y)
{
    UpdateScrollbarHover(mouse_x, mouse_y);
    if (is_dragging_window)
    {
        window_x = mouse_x - drag_offset_x;
        window_y = mouse_y - drag_offset_y;
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
    if (!(mouse_y <= this->window_y + 43 || mouse_y >= this->window_y + 243 || mouse_x <= this->window_x + 17 ||
        mouse_x >= this->window_x + 252 || this->is_dragging_window))
    {
        int row = (mouse_y - window_y - 47) / NPC_TEXT_LINE_HEIGHT;
        if (row > 11)
            row = -1;
        if (!list_kind && friend_names->Count <= row)
            row = -1;
        if (list_kind == 1 && ignored_names->Count <= row)
            row = -1;
        if (row != hover_row)
        {
            hover_row = row;
            MainForm->redraw_needed = true;
        }
    }
    else
    {
        if (this->hover_row >= 0)
        {
            this->hover_row = -1;
            MainForm->redraw_needed = true;
        }
    }
    if (!(mouse_x < this->window_x + 48 || mouse_x > this->window_x + 138 || mouse_y < this->window_y + 253 ||
        mouse_y > this->window_y + 279))
    {
        if (!this->add_button_hover)
        {
            this->add_button_hover = true;
            MainForm->redraw_needed = true;
        }
    }
    else
    {
        if (add_button_hover)
        {
            add_button_hover = false;
            MainForm->redraw_needed = true;
        }
    }
    if (!(mouse_x < this->window_x + 144 || mouse_x > this->window_x + 234 || mouse_y < this->window_y + 253 ||
        mouse_y > this->window_y + 279))
    {
        if (!this->close_button_hover)
        {
            this->close_button_hover = true;
            MainForm->redraw_needed = true;
        }
    }
    else
    {
        if (close_button_hover)
        {
            close_button_hover = false;
            MainForm->redraw_needed = true;
        }
    }
}

// Drags the thumb while it is held, and lights the arrow under the mouse.
void SocialController::UpdateScrollbarHover(int mouse_x, int mouse_y)
{
    if (mouse_x >= this->window_x + 252 && mouse_x <= this->window_x + 267)
    {
        if (mouse_y >= this->window_y + 58 && mouse_y <= this->window_y + 229)
            this->DragScrollbar(mouse_y);
    }
    if (mouse_x >= window_x + 252 && mouse_x <= window_x + 267 && mouse_y >= window_y + 44 && mouse_y <= window_y + 59)
    {
        if (!scroll_up_hover)
        {
            scroll_up_hover = 1;
            MainForm->redraw_needed = true;
        }
    }
    else if (scroll_up_hover)
    {
        scroll_up_hover = 0;
        MainForm->redraw_needed = true;
    }
    if (mouse_x >= window_x + 252 && mouse_x <= window_x + 267 && mouse_y >= window_y + 228 &&
        mouse_y <= window_y + 243)
    {
        if (!scroll_down_hover)
        {
            scroll_down_hover = 1;
            MainForm->redraw_needed = true;
        }
    }
    else if (scroll_down_hover)
    {
        scroll_down_hover = 0;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the list to match the thumb: 152 pixels of track over the names past the 12 shown.
void SocialController::DragScrollbar(int mouse_y)
{
    double thumb_ratio;
    double scaled_offset;
    int __slot1;
    if (is_dragging_thumb)
    {
        int thumb_position = mouse_y - thumb_grab_offset - window_y;
        if (thumb_position < 0)
            thumb_position = 0;
        if (thumb_position > 152)
            thumb_position = 152;
        int name_count = friend_names->Count;
        if (list_kind == 1)
            name_count = ignored_names->Count;
        if (name_count > 12)
        {
            thumb_ratio = thumb_position / NPC_THUMB_TRAVEL;
            scaled_offset = (name_count - 12.0) * thumb_ratio;
            int new_offset = scaled_offset;
            if (new_offset != scroll_offset)
                scroll_offset = new_offset;
        }
        scrollbar_position = thumb_position;
        scroll_moved_flag = true;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the list up a name and moves the thumb to match.
void SocialController::ScrollUp()
{
    double offset_ratio;
    double thumb_position;
    int name_count = friend_names->Count;
    if (list_kind == 1)
        name_count = ignored_names->Count;
    if (name_count > 12 && scroll_offset > 0)
    {
        scroll_offset--;
        offset_ratio = (double)scroll_offset / (double)(name_count - 12);
        thumb_position = NPC_THUMB_TRAVEL * offset_ratio;
        scrollbar_position = thumb_position;
        scroll_moved_flag = true;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the list down a name and moves the thumb to match.
void SocialController::ScrollDown()
{
    double offset_ratio;
    double thumb_position;
    int result = friend_names->Count;
    if (list_kind == 1)
        result = ignored_names->Count;
    if (result - 12 > scroll_offset)
    {
        scroll_offset++;
        offset_ratio = (double)scroll_offset / (double)(result - 12);
        thumb_position = NPC_THUMB_TRAVEL * offset_ratio;
        scrollbar_position = thumb_position;
        scroll_moved_flag = true;
        MainForm->redraw_needed = true;
    }
}

// Ends every drag and arrow scroll when the button is let go.
void SocialController::StopDragging()
{
    is_dragging_window = 0;
    scrolling_up = 0;
    scrolling_down = 0;
    is_dragging_thumb = 0;
}
