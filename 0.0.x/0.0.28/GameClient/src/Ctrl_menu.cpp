#include <vcl.h>
#pragma hdrstop

#include "Ctrl_menu.h"
#include "Tmainform.h"
#include <stdlib.h>
#pragma package(smart_init)

#define GRID_PASSWORD_LIMIT 15
#define MENU_THUMB_TRAVEL 72.0

MenuController::MenuController()
{
    is_loading_world = false;
    is_entering_game = 0;
    request_delay_ticks = 0;
    create_account_enabled = false;
    login_enabled = false;
    change_password_enabled = false;
    create_character_enabled = false;
    character_select_cooldown = 0;
    connect_for_create_account = 0;
    connect_for_login = 0;
    create_account_button_hover = false;
    play_button_hover = false;
    credits_button_hover = false;
    exit_button_hover = false;
    account_create_button_hover = false;
    account_cancel_button_hover = false;
    change_password_button_hover = false;
    new_character_button_hover = false;
    first_slot_login_hover = false;
    second_slot_login_hover = false;
    third_slot_login_hover = false;
    first_slot_delete_hover = false;
    second_slot_delete_hover = false;
    third_slot_delete_hover = false;
    text_scroll_up_hover = false;
    text_scroll_down_hover = false;
    gender_arrow_hover = false;
    hair_style_arrow_hover = false;
    hair_color_arrow_hover = false;
    skin_arrow_hover = false;
    corner_close_hover = false;
    message_close_button_hover = false;
    text_window_ok_button_hover = false;
    message_confirm_button_hover = false;
    form_ok_button_hover = false;
    form_cancel_button_hover = false;
    login_ok_button_hover = false;
    login_cancel_button_hover = false;
    unread_flag_at_48 = false;
    unread_int_at_52 = 0;
    is_message_open = false;
    message_x = DEFAULT_MESSAGE_BOX_X;
    message_y = DEFAULT_MESSAGE_BOX_Y;
    message_lines[0] = "";
    message_lines[1] = "";
    is_text_window_open = false;
    text_scroll_offset = 0;
    text_scrollbar_position = 0;
    text_window_x = 140;
    text_window_y = 110;
    scrolling_up = 0;
    scrolling_down = 0;
    is_form_open = false;
    form_x = DEFAULT_FORM_X;
    form_y = DEFAULT_FORM_Y;
    text_lines = new TStringList();
    is_dragging_window = 0;
    is_dragging_thumb = 0;
    new_gender = 0;
    new_hair_style = 1;
    new_hair_color = 0;
    new_skin = 0;
    map_needed = 0;
    item_file_needed = 0;
    reserved_file_needed = 0;
    class_file_needed = 0;
    npc_file_needed = 0;
    skill_file_needed = 0;
    up_key_lit = false;
    down_key_lit = false;
    left_key_lit = false;
    right_key_lit = false;
    space_key_lit = false;
    select_key_lit = false;
    delete_key_lit = false;
    letter_delay_ticks = 0;
    retry_ticks = 0;
}

MenuController::~MenuController()
{
    text_lines->Clear();
}

// Counts down the scroll repeat, the letter pad delay and the retry delay, and blinks the letter pad cursor.
void MenuController::OnTimer()
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
    cursor_blink++;
    if (cursor_blink > 8)
        cursor_blink = 0;
    if (letter_delay_ticks > 0)
        letter_delay_ticks--;
    if (retry_ticks > 0)
        retry_ticks--;
}

// Opens the form window of this kind and clears the hover of the buttons under it.
void MenuController::OpenFormWindow(int kind)
{
    is_form_open = true;
    is_dragging_window = 0;
    form_kind = kind;
    change_password_button_hover = false;
    new_character_button_hover = false;
    first_slot_login_hover = false;
    second_slot_login_hover = false;
    third_slot_login_hover = false;
    first_slot_delete_hover = false;
    second_slot_delete_hover = false;
    third_slot_delete_hover = false;
}

// Opens the text window empty and at the top; AddWrappedText fills it.
void MenuController::OpenTextWindow()
{
    text_lines->Clear();
    text_scroll_offset = 0;
    text_scrollbar_position = 0;
    is_text_window_open = true;
    is_dragging_window = 0;
    form_ok_button_hover = false;
    form_cancel_button_hover = false;
    gender_arrow_hover = false;
    hair_style_arrow_hover = false;
    hair_color_arrow_hover = false;
    skin_arrow_hover = false;
    create_account_button_hover = false;
    play_button_hover = false;
    credits_button_hover = false;
    exit_button_hover = false;
    account_create_button_hover = false;
    account_cancel_button_hover = false;
    change_password_button_hover = false;
    new_character_button_hover = false;
    first_slot_login_hover = false;
    second_slot_login_hover = false;
    third_slot_login_hover = false;
    first_slot_delete_hover = false;
    second_slot_delete_hover = false;
    third_slot_delete_hover = false;
    login_ok_button_hover = false;
    login_cancel_button_hover = false;
}

// Adds text to the text window, cut at spaces into lines of at most 280 pixels in MS Sans Serif 7.
// An empty line follows when add_blank_line is set.
void MenuController::AddWrappedText(String text, bool add_blank_line)
{
    int max_width;
    HDC dc;
    max_width = 280;
    if (MainForm->OffscreenSurface->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Font->Name = "MS Sans Serif";
            MainForm->draw_canvas->Font->Size = 7;
            do
            {
                String fitting_part;
                if (MainForm->draw_canvas->TextWidth(text) <= max_width)
                {
                    text_lines->Add(text);
                    text = "";
                }
                else
                {
                    char line_cut = 0;
                    for (int i = 1; i <= text.Length(); i++)
                    {
                        if (text[i] != ' ' && i != text.Length())
                            continue;
                        String prefix = text.SubString(1, i);
                        if (MainForm->draw_canvas->TextWidth(prefix) >= max_width)
                        {
                            text_lines->Add(fitting_part);
                            text.Delete(1, fitting_part.Length());
                            line_cut = 1;
                            break;
                        }
                        fitting_part = prefix;
                    }
                    if (!line_cut)
                    {
                        text_lines->Add(text);
                        text = "";
                    }
                }
            } while (text.Length() > 0);
            MainForm->draw_canvas->Handle = 0;
        }
        catch (Exception &)
        {
        }
        MainForm->OffscreenSurface->ReleaseDC(dc);
    }
    if (add_blank_line)
        text_lines->Add("");
}

int MenuController::GetScrollOffset()
{
    return text_scroll_offset;
}

int MenuController::GetScrollbarPosition()
{
    return text_scrollbar_position;
}

// Opens the message box with a title and one line of text.
void MenuController::ShowMessageBox(int kind, String title, String text)
{
    is_message_open = true;
    message_shows_image = false;
    is_dragging_window = 0;
    message_lines[0] = text;
    message_lines[1] = "";
    message_kind = kind;
    message_title = title;
    form_ok_button_hover = false;
    form_cancel_button_hover = false;
    gender_arrow_hover = false;
    hair_style_arrow_hover = false;
    hair_color_arrow_hover = false;
    skin_arrow_hover = false;
    create_account_button_hover = false;
    play_button_hover = false;
    credits_button_hover = false;
    exit_button_hover = false;
    account_create_button_hover = false;
    account_cancel_button_hover = false;
    change_password_button_hover = false;
    new_character_button_hover = false;
    first_slot_login_hover = false;
    second_slot_login_hover = false;
    third_slot_login_hover = false;
    first_slot_delete_hover = false;
    second_slot_delete_hover = false;
    third_slot_delete_hover = false;
    login_ok_button_hover = false;
    login_cancel_button_hover = false;
}

void MenuController::SetEnteringGame(char entering)
{
    is_entering_game = entering;
}

char MenuController::IsEnteringGame()
{
    return is_entering_game;
}

void MenuController::StartRequestDelay()
{
    request_delay_ticks = 1;
}

// True once, 24 calls after StartRequestDelay; the loading screen then asks for the next game data.
bool MenuController::IsRequestDelayDone()
{
    bool is_done = 0;
    if (request_delay_ticks > 0)
    {
        request_delay_ticks++;
        if (request_delay_ticks > 24)
        {
            is_done = 1;
            request_delay_ticks = 0;
        }
    }
    return is_done;
}

void MenuController::SetMapNeeded(char needed)
{
    map_needed = needed;
}

void MenuController::SetReservedFileNeeded(char needed)
{
    reserved_file_needed = needed;
}

void MenuController::SetClassFileNeeded(char needed)
{
    class_file_needed = needed;
}

void MenuController::SetItemFileNeeded(char needed)
{
    item_file_needed = needed;
}

void MenuController::SetNpcFileNeeded(char needed)
{
    npc_file_needed = needed;
}

void MenuController::SetSkillFileNeeded(char needed)
{
    skill_file_needed = needed;
}

char MenuController::IsMapNeeded()
{
    return map_needed;
}

char MenuController::IsReservedFileNeeded()
{
    return reserved_file_needed;
}

char MenuController::IsClassFileNeeded()
{
    return class_file_needed;
}

char MenuController::IsItemFileNeeded()
{
    return item_file_needed;
}

char MenuController::IsNpcFileNeeded()
{
    return npc_file_needed;
}

char MenuController::IsSkillFileNeeded()
{
    return skill_file_needed;
}

// Starts a drag of the scrollbar thumb, an arrow scroll, or a drag of the window under the mouse.
// Window sizes: message box 290 x 157, form and text window 360 x 240.
void MenuController::OnMouseDown(int mouse_x, int mouse_y)
{
    if (is_text_window_open)
    {
        if (mouse_x >= text_window_x + 320 && mouse_x <= text_window_x + 335)
        {
            if (mouse_y >= text_window_y + text_scrollbar_position + 81 &&
                mouse_y <= text_window_y + text_scrollbar_position + 96)
            {
                is_dragging_thumb = 1;
                thumb_grab_offset = mouse_y - text_scrollbar_position - text_window_y;
                DragScrollbar(mouse_y);
            }
        }
    }
    if (is_form_open && mouse_x >= form_x && mouse_x <= form_x + 360 && mouse_y >= form_y && mouse_y <= form_y + 240)
    {
        is_dragging_window = 1;
        dragged_window = DraggedWindow_Form;
        drag_offset_x = mouse_x - form_x;
        drag_offset_y = mouse_y - form_y;
    }
    if (is_text_window_open && mouse_x >= text_window_x && mouse_x <= text_window_x + 360 && mouse_y >= text_window_y &&
        mouse_y <= text_window_y + 240)
    {
        if (mouse_x >= text_window_x + 320 && mouse_x <= text_window_x + 335 && mouse_y >= text_window_y + 66 &&
            mouse_y <= text_window_y + 81)
        {
            scrolling_up = 1;
            scroll_repeat_delay = 4;
            ScrollUp();
        }
        if (mouse_x >= text_window_x + 320 && mouse_x <= text_window_x + 335 && mouse_y >= text_window_y + 170 &&
            mouse_y <= text_window_y + 185)
        {
            scrolling_down = 1;
            scroll_repeat_delay = 4;
            ScrollDown();
        }
        if (!is_dragging_thumb)
        {
            is_dragging_window = 1;
            dragged_window = DraggedWindow_TextWindow;
            drag_offset_x = mouse_x - text_window_x;
            drag_offset_y = mouse_y - text_window_y;
        }
    }
    if (is_message_open && mouse_x >= message_x && mouse_x <= message_x + 290 && mouse_y >= message_y &&
        mouse_y <= message_y + 157)
    {
        is_dragging_window = 1;
        dragged_window = DraggedWindow_MessageBox;
        drag_offset_x = mouse_x - message_x;
        drag_offset_y = mouse_y - message_y;
    }
}

// Moves the window being dragged, kept on the 640 x 480 screen.
void MenuController::OnMouseMove(int mouse_x, int mouse_y)
{
    UpdateScrollbarHover(mouse_x, mouse_y);
    if (is_dragging_window && dragged_window == DraggedWindow_TextWindow)
    {
        text_window_x = mouse_x - drag_offset_x;
        text_window_y = mouse_y - drag_offset_y;
        if (text_window_x <= 0)
            text_window_x = 0;
        if (text_window_y <= 0)
            text_window_y = 0;
        if (text_window_x >= 280)
            text_window_x = 280;
        if (text_window_y >= 240)
            text_window_y = 240;
    }
    if (is_dragging_window && dragged_window == DraggedWindow_Form)
    {
        form_x = mouse_x - drag_offset_x;
        form_y = mouse_y - drag_offset_y;
        if (form_x <= 0)
            form_x = 0;
        if (form_y <= 0)
            form_y = 0;
        if (form_x >= 280)
            form_x = 280;
        if (form_y >= 240)
            form_y = 240;
    }
    if (is_dragging_window && dragged_window == DraggedWindow_MessageBox)
    {
        message_x = mouse_x - drag_offset_x;
        message_y = mouse_y - drag_offset_y;
        if (message_x <= 0)
            message_x = 0;
        if (message_y <= 0)
            message_y = 0;
        if (message_x >= 350)
            message_x = 350;
        if (message_y >= 323)
            message_y = 323;
    }
}

// Drags the thumb while it is held, and lights the arrow under the mouse.
void MenuController::UpdateScrollbarHover(int mouse_x, int mouse_y)
{
    if (is_text_window_open)
    {
        if (mouse_x >= text_window_x + 320 && mouse_x <= text_window_x + 335)
        {
            if (mouse_y >= text_window_y + 81 && mouse_y <= text_window_y + 166)
                DragScrollbar(mouse_y);
        }
        if (mouse_x >= text_window_x + 320 && mouse_x <= text_window_x + 335 && mouse_y >= text_window_y + 66 &&
            mouse_y <= text_window_y + 81)
        {
            if (!text_scroll_up_hover)
                text_scroll_up_hover = true;
        }
        else if (text_scroll_up_hover)
            text_scroll_up_hover = false;
        if (mouse_x >= text_window_x + 320 && mouse_x <= text_window_x + 335 && mouse_y >= text_window_y + 170 &&
            mouse_y <= text_window_y + 185)
        {
            if (!text_scroll_down_hover)
                text_scroll_down_hover = true;
        }
        else if (text_scroll_down_hover)
            text_scroll_down_hover = false;
    }
}

// Scrolls the text window to match the thumb: 72 pixels of track over the lines past the 7 shown.
void MenuController::DragScrollbar(int mouse_y)
{
    double thumb_ratio;
    double scaled_offset;
    int __slot1;
    if (is_dragging_thumb)
    {
        int thumb_position = mouse_y - thumb_grab_offset - text_window_y;
        if (thumb_position < 0)
            thumb_position = 0;
        if (thumb_position > 72)
            thumb_position = 72;
        int line_count = text_lines->Count;
        if (line_count > 7)
        {
            thumb_ratio = thumb_position / 72.0;
            scaled_offset = (line_count - 7.0) * thumb_ratio;
            int new_offset = scaled_offset;
            if (new_offset != text_scroll_offset)
                text_scroll_offset = new_offset;
        }
        text_scrollbar_position = thumb_position;
    }
}

// Scrolls the text window up a line and moves the thumb to match.
void MenuController::ScrollUp()
{
    double offset_ratio;
    double thumb_position;
    int line_count = text_lines->Count;
    if (line_count > 7 && text_scroll_offset > 0)
    {
        text_scroll_offset--;
        offset_ratio = (double)text_scroll_offset / (double)(line_count - 7);
        thumb_position = MENU_THUMB_TRAVEL * offset_ratio;
        text_scrollbar_position = thumb_position;
    }
}

// Scrolls the text window down a line and moves the thumb to match.
void MenuController::ScrollDown()
{
    double offset_ratio;
    double thumb_position;
    int line_count = text_lines->Count;
    if (line_count - 7 > text_scroll_offset)
    {
        text_scroll_offset++;
        offset_ratio = (double)text_scroll_offset / (double)(line_count - 7);
        thumb_position = MENU_THUMB_TRAVEL * offset_ratio;
        text_scrollbar_position = thumb_position;
    }
}

// Adds the letter under the letter pad cursor to the password, 16 letters at most.
// After every second letter the cursor jumps to a random cell of rows and columns 1 to 4.
// row 0 = a b c d e f
// row 1 = g h i j k l
// row 2 = m n o p q r
// row 3 = s t u v w x
// row 4 = y z 0 1 2 3
// row 5 = 4 5 6 7 8 9
void MenuController::AddSelectedLetter()
{
    if (password.Length() > GRID_PASSWORD_LIMIT)
        return;
    if (!grid_row)
    {
        if (!grid_column)
            password = password + "a";
        if (grid_column == 1)
            password = password + "b";
        if (grid_column == 2)
            password = password + "c";
        if (grid_column == 3)
            password = password + "d";
        if (grid_column == 4)
            password = password + "e";
        if (grid_column == 5)
            password = password + "f";
    }
    if (grid_row == 1)
    {
        if (!grid_column)
            password = password + "g";
        if (grid_column == 1)
            password = password + "h";
        if (grid_column == 2)
            password = password + "i";
        if (grid_column == 3)
            password = password + "j";
        if (grid_column == 4)
            password = password + "k";
        if (grid_column == 5)
            password = password + "l";
    }
    if (grid_row == 2)
    {
        if (!grid_column)
            password = password + "m";
        if (grid_column == 1)
            password = password + "n";
        if (grid_column == 2)
            password = password + "o";
        if (grid_column == 3)
            password = password + "p";
        if (grid_column == 4)
            password = password + "q";
        if (grid_column == 5)
            password = password + "r";
    }
    if (grid_row == 3)
    {
        if (!grid_column)
            password = password + "s";
        if (grid_column == 1)
            password = password + "t";
        if (grid_column == 2)
            password = password + "u";
        if (grid_column == 3)
            password = password + "v";
        if (grid_column == 4)
            password = password + "w";
        if (grid_column == 5)
            password = password + "x";
    }
    if (grid_row == 4)
    {
        if (!grid_column)
            password = password + "y";
        if (grid_column == 1)
            password = password + "z";
        if (grid_column == 2)
            password = password + "0";
        if (grid_column == 3)
            password = password + "1";
        if (grid_column == 4)
            password = password + "2";
        if (grid_column == 5)
            password = password + "3";
    }
    if (grid_row == 5)
    {
        if (!grid_column)
            password = password + "4";
        if (grid_column == 1)
            password = password + "5";
        if (grid_column == 2)
            password = password + "6";
        if (grid_column == 3)
            password = password + "7";
        if (grid_column == 4)
            password = password + "8";
        if (grid_column == 5)
            password = password + "9";
    }
    letter_delay_ticks = 4;
    if (!(password.Length() % 2))
    {
        MainForm->sfx->PlaySfx(3);
        grid_column = random(4) + 1;
        grid_row = random(4) + 1;
    }
    else
        MainForm->sfx->PlaySfx(2);
}

void MenuController::AddSpace()
{
    if (password.Length() > GRID_PASSWORD_LIMIT)
        return;
    password = password + " ";
    return;
}

void MenuController::DeleteLastLetter()
{
    if (password.Length() > 0)
        password = password.SubString(1, password.Length() - 1);
}

// Ends every drag and arrow scroll when the button is let go.
void MenuController::StopDragging()
{
    scrolling_up = 0;
    scrolling_down = 0;
    is_dragging_thumb = 0;
    is_dragging_window = 0;
}
