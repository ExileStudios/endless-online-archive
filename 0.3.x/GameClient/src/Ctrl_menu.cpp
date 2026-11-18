#include <vcl.h>
#pragma hdrstop

#include "Ctrl_menu.h"
#include "Tmainform.h"
#include <stdlib.h>
#include "Gfx_menu.h"

#pragma package(smart_init)

// Starts the menu with every button, form and request flag cleared.
MenuController::MenuController()
{
    is_loading_world = false;
    is_entering_game = false;
    request_delay_ticks = 0;
    create_account_enabled = false;
    login_enabled = false;
    change_password_enabled = false;
    create_character_enabled = false;
    character_select_cooldown = 0;
    connect_for_create_account = 0;
    connect_for_login = false;
    connect_for_recovery = false;
    create_account_button_hover = false;
    play_button_hover = false;
    credits_button_hover = false;
    exit_button_hover = false;
    account_create_button_hover = false;
    account_cancel_button_hover = false;
    recover_pending = false;
    wide_confirm_hover = false;
    wide_cancel_hover = false;
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
    confirm_ok_hover = false;
    login_ok_button_hover = false;
    login_cancel_button_hover = false;
    intro_finished = false;
    unused_value = 0;
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
    scrolling_up = false;
    scrolling_down = false;
    is_form_open = false;
    form_x = 100;
    form_y = 112;
    text_lines = new TStringList();
    is_dragging_window = false;
    is_dragging_thumb = false;
    title_idle_ticks = 40;
    title_idle_cycles = 0;
    new_gender = 0;
    new_hair_style = 1;
    new_hair_color = 0;
    new_skin = 0;
    skin_count = 4;
    hair_style_count = 20;
    map_needed = false;
    item_file_needed = false;
    reserved_file_needed = 0;
    class_file_needed = false;
    npc_file_needed = false;
    skill_file_needed = false;
    up_key_lit = false;
    down_key_lit = false;
    left_key_lit = false;
    right_key_lit = false;
    space_key_lit = false;
    select_key_lit = false;
    delete_key_lit = false;
    letter_delay_ticks = 0;
    retry_ticks = 0;
    recover_step = 0;
    activation_step = 0;
}

MenuController::~MenuController()
{
    text_lines->Clear();
}

// One menu timer tick: held scroll buttons, the title screen's background change, the caret blink, and the account
// activation and recover steps; a recover request with no reply in time says the server does not support it.
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
    if (MainForm->animation->GetSubState() == 1)
    {
        title_idle_ticks++;
        if (title_idle_ticks > 48)
        {
            title_idle_cycles++;
            if (title_idle_cycles > 4)
            {
                MainForm->gfx->NextMenuBackground();
                title_idle_ticks = 0;
                title_idle_cycles = 0;
            }
        }
    }
    else
        title_idle_ticks = 0;
    cursor_blink++;
    if (cursor_blink > 8)
        cursor_blink = 0;
    if (letter_delay_ticks > 0)
        letter_delay_ticks--;
    if (retry_ticks > 0)
        retry_ticks--;
    if (recover_pending)
    {
        if (DateTimeToTimeStamp(Now()).Time - recover_request_time.Time > 2200)
        {
            recover_pending = false;
            ShowMessageBox(MessageBoxKind_RecoverNotice, "Account Recover",
                "The account recover feature is not supported on this server.");
        }
    }
    if (activation_step == 1)
    {
        if (activation_wait > 0)
            activation_wait--;
        else
        {
            activation_step = 2;
            ShowActivationStep(1);
        }
    }
    if (recover_step == 1)
    {
        if (recover_wait > 0)
            recover_wait--;
        else
        {
            recover_step = 2;
            ShowRecoverStep(2);
        }
    }
}

void MenuController::StartRecoverTimeout()
{
    recover_pending = true;
    recover_request_time = DateTimeToTimeStamp(Now());
}

// Opens the form window of this kind, resetting the new character choices and the hover of the buttons under it.
void MenuController::OpenFormWindow(int kind)
{
    is_form_open = true;
    is_dragging_window = false;
    form_kind = kind;
    change_password_button_hover = false;
    new_character_button_hover = false;
    new_gender = 0;
    new_hair_style = 1;
    new_hair_color = 0;
    hair_page = 0;
    left_arrow_state = 0;
    right_arrow_state = 0;
    first_slot_login_hover = false;
    second_slot_login_hover = false;
    third_slot_login_hover = false;
    first_slot_delete_hover = false;
    second_slot_delete_hover = false;
    third_slot_delete_hover = false;
}

// Opens the text window empty and at the top, and clears the hover of every menu button.
void MenuController::OpenTextWindow()
{
    text_lines->Clear();
    text_scroll_offset = 0;
    text_scrollbar_position = 0;
    is_text_window_open = true;
    is_dragging_window = false;
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
    wide_confirm_hover = false;
    wide_cancel_hover = false;
    change_password_button_hover = false;
    new_character_button_hover = false;
    first_slot_login_hover = false;
    second_slot_login_hover = false;
    third_slot_login_hover = false;
    first_slot_delete_hover = false;
    second_slot_delete_hover = false;
    third_slot_delete_hover = false;
    confirm_ok_hover = false;
    login_ok_button_hover = false;
    login_cancel_button_hover = false;
}

// Adds text to the text window, cut at spaces into lines that fit its width.
// An empty line follows when add_blank_line is set.
void MenuController::AddWrappedText(String text, bool add_blank_line)
{
    int max_width = 280;
    if (MainForm->fonts->BeginOffscreenIf(200))
    {
        try
        {
            MainForm->fonts->face = 1;
            MainForm->fonts->size = 8;
            do
            {
                String fitting_part;
                if ((short)MainForm->fonts->TextWidth(text) <= max_width)
                {
                    text_lines->Add(text);
                    text = "";
                }
                else
                {
                    char line_cut = false;
                    for (int i = 1; i <= text.Length(); i++)
                    {
                        if (text[i] == ' ' || i == text.Length())
                        {
                            String prefix = text.SubString(1, i);
                            if ((short)MainForm->fonts->TextWidth(prefix) >= max_width)
                            {
                                text_lines->Add(fitting_part);
                                text.Delete(1, fitting_part.Length());
                                line_cut = true;
                                break;
                            }
                            fitting_part = prefix;
                        }
                    }
                    if (!line_cut)
                    {
                        text_lines->Add(text);
                        text = "";
                    }
                }
            } while (text.Length() > 0);
        }
        catch (Exception &)
        {
        }
        MainForm->fonts->End();
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
    is_dragging_window = false;
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
    wide_confirm_hover = false;
    wide_cancel_hover = false;
    change_password_button_hover = false;
    new_character_button_hover = false;
    first_slot_login_hover = false;
    second_slot_login_hover = false;
    third_slot_login_hover = false;
    first_slot_delete_hover = false;
    second_slot_delete_hover = false;
    third_slot_delete_hover = false;
    confirm_ok_hover = false;
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
    bool is_done = false;
    if (request_delay_ticks > 0)
    {
        request_delay_ticks++;
        if (request_delay_ticks > 24)
        {
            is_done = true;
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
                is_dragging_thumb = true;
                thumb_grab_offset = mouse_y - text_scrollbar_position - text_window_y;
                DragScrollbar(mouse_y);
            }
        }
    }
    if (is_form_open && mouse_x >= form_x && mouse_x <= form_x + 465 && mouse_y >= form_y && mouse_y <= form_y + 257)
    {
        is_dragging_window = true;
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
            scrolling_up = true;
            scroll_repeat_delay = 4;
            ScrollUp();
        }
        if (mouse_x >= text_window_x + 320 && mouse_x <= text_window_x + 335 && mouse_y >= text_window_y + 170 &&
            mouse_y <= text_window_y + 185)
        {
            scrolling_down = true;
            scroll_repeat_delay = 4;
            ScrollDown();
        }
        if (!is_dragging_thumb)
        {
            is_dragging_window = true;
            dragged_window = DraggedWindow_TextWindow;
            drag_offset_x = mouse_x - text_window_x;
            drag_offset_y = mouse_y - text_window_y;
        }
    }
    if (is_message_open && mouse_x >= message_x && mouse_x <= message_x + 290 && mouse_y >= message_y &&
        mouse_y <= message_y + 157)
    {
        is_dragging_window = true;
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
        if (form_x >= 171)
            form_x = 171;
        if (form_y >= 220)
            form_y = 220;
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
        DragScrollbar(mouse_y);
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
    int new_offset;
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
            double thumb_ratio = thumb_position / 72.0;
            double scaled_offset = (line_count - 7.0) * thumb_ratio;
            new_offset = scaled_offset;
            if (new_offset != text_scroll_offset)
                text_scroll_offset = new_offset;
        }
        text_scrollbar_position = thumb_position;
    }
}

// Scrolls the text window up a line and moves the thumb to match.
void MenuController::ScrollUp()
{
    int line_count = text_lines->Count;
    if (line_count > 7 && text_scroll_offset > 0)
    {
        text_scroll_offset--;
        double offset_ratio = (double)text_scroll_offset / (line_count - 7);
        double thumb_position = MENU_THUMB_TRAVEL * offset_ratio;
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
        double offset_ratio = (double)text_scroll_offset / (line_count - 7);
        double thumb_position = MENU_THUMB_TRAVEL * offset_ratio;
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
    // A password holds at most 16 letters.
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
    if (password.Length() % 2 == 0)
    {
        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
        grid_column = random(4) + 1;
        grid_row = random(4) + 1;
    }
    else
        MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
}

void MenuController::AddSpace()
{
    if (password.Length() > GRID_PASSWORD_LIMIT)
        return;
    password = password + " ";
}

void MenuController::DeleteLastLetter()
{
    if (password.Length() > 0)
        password = password.SubString(1, password.Length() - 1);
}

// Ends every drag and arrow scroll when the button is let go.
void MenuController::StopDragging()
{
    scrolling_up = false;
    scrolling_down = false;
    is_dragging_thumb = false;
    is_dragging_window = false;
}

// Shows the sign-up code screen for the step; step 1 adds the box for the 7-character code.
void MenuController::ShowActivationStep(int step)
{
    if (!step)
    {
        MainForm->edits->Clear();
        activation_step = 1;
        activation_wait = 40;
        MainForm->animation->menu->create_account_enabled = true;
        MainForm->animation->SetSubState(MenuScreen_SignupCode);
        MainForm->gfx->LoadSignupCodeScreen(step, false);
    }
    if (step == 1)
    {
        MainForm->gfx->menu->email_text_set = false;
        MainForm->edits->Clear();
        MainForm->animation->SetSubState(MenuScreen_SignupCode);
        MainForm->gfx->LoadSignupCodeScreen(step, false);
        MainForm->edits->AddEdit(EditId_Code, EditGroup_Menu, ACCOUNT_FORM_EDIT_X, 320, 80, EDIT_LINE_HEIGHT);
        MainForm->edits->SetAllowSpaces(EditId_Code, false);
        MainForm->edits->SetCharFilter(EditId_Code, EditCharFilter_Alphanumeric);
        MainForm->edits->SetCaseMode(EditId_Code, EditCaseMode_Code);
        MainForm->edits->SetMaxLength(EditId_Code, 7);
        MainForm->edits->caret_enabled = true;
        MainForm->edits->FocusEdit(EditId_Code);
        MainForm->animation->menu->create_account_enabled = true;
    }
}

// Shows the account recover screen at the step and adds that step's boxes, the last one a password box with its
// strength shown.
void MenuController::ShowRecoverStep(int step)
{
    recover_pending = false;
    MainForm->edits->Clear();
    if (!step)
    {
        recover_step = 0;
        recover_wait = 70;
        MainForm->animation->SetSubState(MenuScreen_PasswordReset);
        MainForm->gfx->LoadPasswordResetScreen(1, true);
        MainForm->edits->AddEdit(EditId_AccountName, EditGroup_Menu, ACCOUNT_FORM_EDIT_X, 300, 242, EDIT_LINE_HEIGHT);
        MainForm->edits->SetAllowSpaces(EditId_AccountName, false);
        MainForm->edits->SetCharFilter(EditId_AccountName, EditCharFilter_Alphanumeric);
        MainForm->edits->SetMaxLength(EditId_AccountName, 16);
        MainForm->edits->caret_enabled = true;
        MainForm->edits->FocusEdit(EditId_AccountName);
    }
    if (step == 2)
    {
        recover_step = 2;
        MainForm->animation->SetSubState(MenuScreen_PasswordReset);
        MainForm->gfx->LoadPasswordResetScreen(2, false);
        MainForm->edits->AddEdit(EditId_Code, EditGroup_Menu, ACCOUNT_FORM_EDIT_X, 300, 80, EDIT_LINE_HEIGHT);
        MainForm->edits->SetAllowSpaces(EditId_Code, false);
        MainForm->edits->SetCharFilter(EditId_Code, EditCharFilter_Alphanumeric);
        MainForm->edits->SetCaseMode(EditId_Code, EditCaseMode_Code);
        MainForm->edits->SetMaxLength(EditId_Code, 7);
        MainForm->edits->caret_enabled = true;
        MainForm->edits->FocusEdit(EditId_Code);
    }
    if (step == 3)
    {
        recover_step = 3;
        MainForm->animation->SetSubState(MenuScreen_PasswordReset);
        MainForm->gfx->LoadPasswordResetScreen(3, false);
        MainForm->edits->AddEdit(EditId_AccountPassword, EditGroup_Menu, ACCOUNT_FORM_EDIT_X, 300, 242,
            EDIT_LINE_HEIGHT);
        MainForm->edits->AddEdit(EditId_ConfirmPassword, EditGroup_Menu, ACCOUNT_FORM_EDIT_X, LOGIN_PASSWORD_EDIT_Y,
            242, EDIT_LINE_HEIGHT);
        MainForm->edits->SetPasswordMode(EditId_AccountPassword, true);
        MainForm->edits->SetPasswordMode(EditId_ConfirmPassword, true);
        MainForm->edits->SetShowStrength(EditId_AccountPassword, true);
        MainForm->edits->SetAllowSpaces(EditId_AccountPassword, false);
        MainForm->edits->SetAllowSpaces(EditId_ConfirmPassword, false);
        MainForm->edits->SetCharFilter(EditId_AccountPassword, EditCharFilter_Password);
        MainForm->edits->SetCharFilter(EditId_ConfirmPassword, EditCharFilter_Password);
        MainForm->edits->SetMaxLength(EditId_AccountPassword, 16);
        MainForm->edits->SetMaxLength(EditId_ConfirmPassword, 16);
        MainForm->edits->caret_enabled = true;
        MainForm->edits->FocusEdit(EditId_AccountPassword);
    }
}
