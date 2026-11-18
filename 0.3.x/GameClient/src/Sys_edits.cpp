#include <vcl.h>
#pragma hdrstop

#include "Sys_edits.h"

#pragma package(smart_init)

Edits::Edits(int default_color)
{
    default_text_color = default_color;
    current_index = -1;
    text_changed = false;
    caret_enabled = true;
    chat_locked = false;
}

Edits::~Edits()
{
}

void Edits::Clear()
{
    if (GetCount() >= 1)
    {
        edits.clear();
        current_index = -1;
    }
}

// The boxes' slow tick: counts the chat lock down, marks the focused box for a redraw and blinks the caret.
void Edits::TimerTick()
{
    if (chat_locked)
    {
        lock_countdown--;
        if (lock_countdown < 0)
            chat_locked = false;
    }
    tick_redraw = true;
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (it->focused)
        {
            if (chat_locked && !lock_countdown && it->id == EditId_Chat)
            {
                it->visible = true;
                it->editable = true;
            }
            if (it->change_notify && caret_enabled)
                text_changed = true;
            if (it->redraw_ticks > 0)
                it->redraw_ticks--;
            it->dirty = true;
            break;
        }
    }
    if (caret_blink_on)
    {
        caret_blink_on = false;
        return;
    }
    caret_blink_on = true;
}

bool Edits::ConsumeTickRedraw()
{
    bool redraw = false;
    if (tick_redraw)
    {
        tick_redraw = false;
        redraw = true;
    }
    return redraw;
}

// Types the text into the focused edit box a character at a time, then applies the box's letter case rule.
void Edits::TypeText(String text)
{
    vector<Edit>::iterator it;
    if (text.Length() < 1)
        return;
    for (it = edits.begin(); it != edits.end(); it++)
    {
        if (it->focused && it->editable && it->text.Length() < it->max_length)
        {
            String before = it->text;
            for (int i = 1; i <= text.Length(); i++)
                TypeChar(text[i]);
            if (it->text.Length() < 1)
                continue;
            it->redraw_ticks = 2;
            if (it->change_notify && before != it->text)
                text_changed = true;
            // Case modes: 1 and 4 upper case, 2 lower case, 3 a capital first letter,
            // 5 a capital first letter and at most one more capital, never two in a row.
            if (it->case_mode == EditCaseMode_Upper)
                it->text = UpperCase(it->text);
            if (it->case_mode == EditCaseMode_Code)
                it->text = UpperCase(it->text);
            if (it->case_mode == EditCaseMode_Lower)
                it->text = LowerCase(it->text);
            if (it->case_mode == EditCaseMode_CapitalFirst)
                it->text[1] = UpperCase(it->text)[1];
            if (it->case_mode == EditCaseMode_Name)
            {
                it->text[1] = UpperCase(it->text)[1];
                short letter_count = 0;
                char after_space = false;
                for (int k = 1; k <= it->text.Length(); k++)
                {
                    if (it->text[k] >= 'A' && it->text[k] <= 'Z')
                    {
                        if (letter_count > 1 || after_space)
                        {
                            it->text[k] += 32;
                            after_space = false;
                        }
                        else
                        {
                            letter_count++;
                            after_space = true;
                        }
                    }
                    else
                        after_space = false;
                }
            }
            break;
        }
    }
}

// Adds one typed character to the focused edit box when the box's rules allow it: spaces, a number's upper limit,
// the character filter's letters, digits and signs, and at most a few dots.
void Edits::TypeChar(char key)
{
    String *text;
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        // A character goes to the focused box while it is editable and not full.
        if (it->focused && it->editable && it->text.Length() < it->max_length)
        {
            if (key == ' ')
            {
                if (it->no_leading_space && it->text.Length() < 1)
                    return;
                if (it->no_double_spaces && it->text.Length() > 0)
                {
                    text = &it->text;
                    if ((*text)[it->text.Length()] == ' ')
                        return;
                }
            }
            it->dirty = true;
            if (it->multi_line)
                it->scroll_offset = 0;
            if (it->numeric)
            {
                if (key >= '0' && key <= '9')
                {
                    if (it->max_value < 10)
                        it->text = String(key);
                    else
                        it->text += String(key);
                    if (StrToInt(it->text) > it->max_value)
                        it->text = IntToStr(it->max_value);
                }
                break;
            }
            if (key == ' ')
            {
                if (it->allow_spaces)
                    it->AppendChar(key);
                break;
            }
            if (!it->char_filter)
            {
                it->AppendChar(key);
                break;
            }
            char allowed = false;
            if (it->char_filter <= EditCharFilter_Letters)
            {
                if (key >= 'A' && key <= 'Z')
                    allowed = true;
                if (key >= 'a' && key <= 'z')
                    allowed = true;
            }
            if (it->char_filter <= EditCharFilter_Alphanumeric)
            {
                if (key >= '0' && key <= '9')
                    allowed = true;
            }
            if (it->char_filter <= EditCharFilter_Password)
            {
                if (key >= '#' && key <= '&')
                    allowed = true;
                if (key == '!')
                    allowed = true;
                else if (key == '_')
                    allowed = true;
                else if (key == '+')
                    allowed = true;
                else if (key == '-')
                    allowed = true;
                else if (key == '*')
                    allowed = true;
                else if (key == '^')
                    allowed = true;
                else if (key == '@')
                    allowed = true;
                else if (key == '&')
                    allowed = true;
                else if (key == '*')
                    allowed = true;
            }
            int filter = it->char_filter;
            if (filter <= EditCharFilter_Email && key == '.')
                allowed = true;
            if (filter <= EditCharFilter_Printable)
            {
                if (key >= '!' && key <= '~')
                    allowed = true;
            }
            if (allowed)
                it->AppendChar(key);
            break;
        }
    }
}

// Enter in the focused box: a multi-line box gets a line break.
void Edits::TypeLineBreak()
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (it->focused && it->editable && it->text.Length() < it->max_length)
        {
            if (it->multi_line)
                it->AppendChar(13);
            if (it->multi_line)
                it->scroll_offset = 0;
            if (it->change_notify)
                text_changed = true;
        }
    }
}

// Clears the focused box's text, or resets a number box to its least value; true when its text changed.
char Edits::ClearFocusedText()
{
    char cleared = false;
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (it->focused && it->editable)
        {
            if (it->text.Length() >= 1)
            {
                it->dirty = true;
                cleared = true;
            }
            if (it->multi_line)
                it->scroll_offset = 0;
            if (it->change_notify)
                text_changed = true;
            // A number box goes back to its least value instead of empty.
            if (it->numeric)
            {
                cleared = false;
                if (it->text != IntToStr(it->min_value))
                {
                    it->text = IntToStr(it->min_value);
                    cleared = true;
                }
            }
            else
                it->ClearText();
            break;
        }
    }
    return cleared;
}

void Edits::Backspace()
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (it->focused && it->editable)
        {
            if (it->text.Length() >= 1)
                it->dirty = true;
            if (it->multi_line)
                it->scroll_offset = 0;
            if (it->change_notify)
                text_changed = true;
            it->RemoveLastChar();
            return;
        }
    }
}

// Rates a password from its length and how many kinds of characters it mixes: upper and lower case letters, digits
// and signs.
int Edits::GetPasswordStrength(String password)
{
    unsigned char symbol_count;
    int strength = 0;
    // Each character past the sixth adds 1.
    if (password.Length() > 6)
        strength += password.Length() - 6;
    char has_upper = false;
    for (int i = 1; i <= password.Length(); i++)
    {
        if (password[i] >= 'A' && password[i] <= 'Z')
        {
            has_upper = true;
            strength++;
            break;
        }
    }
    char has_lower = false;
    for (int j = 1; j <= password.Length(); j++)
    {
        if (password[j] >= 'a')
        {
            has_lower = true;
            strength++;
            break;
        }
    }
    unsigned char digit_count = 0;
    for (int k = 1; k <= password.Length(); k++)
    {
        if (password[k] >= '0' && password[k] <= '9')
            digit_count++;
    }
    unsigned char kinds = 0;
    if (digit_count >= 1)
    {
        strength++;
        kinds++;
    }
    if (has_upper)
        kinds++;
    if (has_lower)
        kinds++;
    symbol_count = 0;
    for (int m = 1; m <= password.Length(); m++)
    {
        if (password[m] == '!')
            symbol_count++;
        else if (password[m] == '_')
            symbol_count++;
        else if (password[m] == '+')
            symbol_count++;
        else if (password[m] == '-')
            symbol_count++;
        else if (password[m] == '*')
            symbol_count++;
        else if (password[m] == '^')
            symbol_count++;
        else if (password[m] == '@')
            symbol_count++;
        else if (password[m] == '&')
            symbol_count++;
        else if (password[m] == '*')
            symbol_count++;
    }
    if (symbol_count >= 1)
    {
        strength++;
        kinds++;
    }
    // One kind of character caps the strength at 5, and two at 6, or 7 from 16 characters on;
    // all four kinds add 1.
    if (kinds <= 1 && strength > 5)
        strength = 5;
    if (kinds <= 2)
    {
        char cap = 6;
        if (password.Length() >= 16)
            cap = 7;
        if ((unsigned char)cap < strength)
            strength = (unsigned char)cap;
    }
    if (kinds >= 4)
        strength++;
    if (strength > 10)
        strength = 10;
    return strength;
}

// The first problem with the password typed in the edit box: too short, too few different characters, or no
// uppercase letter, lowercase letter, or digit or sign.
int Edits::GetPasswordProblem(int id)
{
    vector<Edit>::iterator it = edits.begin();
    while (it != edits.end())
    {
        if (id == it->id)
        {
            String password = it->text;
            if (password.Length() < 6)
                return 4;
            if (HasFewDistinctChars(password))
                return 5;
            bool has_upper = false;
            for (int i = 1; i <= password.Length(); i++)
            {
                if (password[i] >= 'A' && password[i] <= 'Z')
                {
                    has_upper = true;
                    break;
                }
            }
            if (!has_upper)
                return -1;
            has_upper = false;
            for (int k = 1; k <= password.Length(); k++)
            {
                if (password[k] >= 'a')
                {
                    has_upper = true;
                    break;
                }
            }
            if (!has_upper)
                return -2;
            has_upper = false;
            for (int j = 1; j <= password.Length(); j++)
            {
                if (password[j] >= '0' && password[j] <= '9')
                {
                    has_upper = true;
                    break;
                }
                if (password[j] >= '0' && password[j] <= '9')
                {
                    has_upper = true;
                    break;
                }
                if (password[j] == '!')
                {
                    has_upper = true;
                    break;
                }
                if (password[j] == '_')
                {
                    has_upper = true;
                    break;
                }
                if (password[j] == '+')
                {
                    has_upper = true;
                    break;
                }
                if (password[j] == '-')
                {
                    has_upper = true;
                    break;
                }
                if (password[j] == '*')
                {
                    has_upper = true;
                    break;
                }
                if (password[j] == '^')
                {
                    has_upper = true;
                    break;
                }
                if (password[j] == '@')
                {
                    has_upper = true;
                    break;
                }
                if (password[j] == '&')
                {
                    has_upper = true;
                    break;
                }
                if (password[j] == '*')
                {
                    has_upper = true;
                    break;
                }
            }
            if (!has_upper)
                return -3;
            return 0;
        }
        it++;
    }
    return -100;
}

// Adds a box, which takes the focus from the others.
void Edits::AddEdit(int id, int screen_group, int x, int y, int width, int height)
{
    UnfocusAll();
    Edit handle(id, screen_group, x, y, width, height);
    handle.text_color = default_text_color;
    edits.insert(edits.end(), handle);
}

void Edits::RemoveEdit(int id)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            if (it->change_notify)
                text_changed = true;
            edits.erase(it);
            break;
        }
    }
}

void Edits::UnfocusAll()
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
        it->focused = false;
}

// Moves the focus to the box that was clicked.
void Edits::FocusClickedEdit(Edit edit_x)
{
    vector<Edit>::iterator it;
    UnfocusAll();
    for (it = edits.begin(); it != edits.end(); it++)
    {
        if (it->id == String(edit_x.id))
        {
            it->focused = true;
            break;
        }
    }
}

void Edits::FocusEdit(int id)
{
    UnfocusAll();
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            it->focused = true;
            break;
        }
    }
}

bool Edits::HasDirtyEdit()
{
    bool dirty = false;
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (it->dirty)
        {
            dirty = true;
            break;
        }
    }
    return dirty;
}

char Edits::ConsumeTextChanged()
{
    char changed = text_changed;
    text_changed = false;
    return changed;
}

bool Edits::IsFocused(int id)
{
    bool focused = false;
    vector<Edit>::iterator it;
    for (it = edits.begin(); it != edits.end(); it++)
    {
        if ((id == it->id))
        {
            if (it->focused)
            {
                focused = true;
                break;
            }
        }
    }
    return focused;
}

bool Edits::ConsumeDirty(int id)
{
    bool dirty = false;
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id && it->dirty)
        {
            it->dirty = false;
            dirty = true;
            break;
        }
    }
    return dirty;
}

// Moves the focus to the next box, round to the first after the last.
void Edits::FocusNext()
{
    char focus_next = false;
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (focus_next)
        {
            it->focused = true;
            focus_next = false;
            break;
        }
        if (it->focused)
        {
            it->focused = false;
            focus_next = true;
        }
    }
    if (focus_next)
        edits[0].focused = true;
}

Edit Edits::GetCurrentEdit()
{
    return edits[current_index];
}

unsigned int Edits::GetCount()
{
    return edits.size();
}

void Edits::MoveNext()
{
    if (GetCount() > current_index)
        current_index++;
}

// With MoveNext, IsAtEnd and GetCurrentEdit, walks the boxes in order.
void Edits::MoveFirst()
{
    if (GetCount() >= 1)
    {
        current_index = 0;
        return;
    }
    current_index = -1;
}

bool Edits::IsAtEnd()
{
    bool at_end = false;
    if (GetCount() == current_index || !GetCount())
        at_end = true;
    return at_end;
}

// Makes the box numeric within the range, with the top value capped.
void Edits::SetNumericRange(int id, int min_value, int max_value)
{
    vector<Edit>::iterator it;
    for (it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            it->numeric = true;
            it->min_value = min_value;
            it->max_value = max_value;
            if (it->max_value > 10000000)
                it->max_value = 10000000;
            break;
        }
    }
}

void Edits::SetCaseMode(int id, int case_mode)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            it->case_mode = case_mode;
            return;
        }
    }
}

void Edits::EnableChangeNotify(int id)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            it->change_notify = true;
            return;
        }
    }
}

void Edits::SetVisible(int id, char visible)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            it->visible = visible;
            return;
        }
    }
}

void Edits::SetTextColor(int id, int color)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            it->text_color = color;
            return;
        }
    }
}

void Edits::SetPosition(int id, int x, int y)
{
    vector<Edit>::iterator it;
    for (it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            it->x = x;
            it->y = y;
            break;
        }
    }
}

void Edits::SetBackgroundVisible(int id, char visible)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            it->background_visible = visible;
            return;
        }
    }
}

void Edits::SetMultiLine(int id, char multi_line)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            it->multi_line = multi_line;
            it->first_visible_line = 0;
            return;
        }
    }
}

int Edits::GetScrollOffset(int id)
{
    int offset = 0;
    vector<Edit>::iterator it;
    for (it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id && it->multi_line)
        {
            offset = it->scroll_offset;
            break;
        }
    }
    return offset;
}

void Edits::SetPendingScroll(int id, int offset)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id && it->multi_line)
        {
            it->pending_scroll = offset;
            return;
        }
    }
}

void Edits::SetScrollOffset(int id, int offset)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id && it->multi_line)
        {
            it->scroll_offset = offset;
            if (it->line_count - it->visible_line_count > 0)
            {
                if (it->scroll_offset > it->line_count - it->visible_line_count)
                    it->scroll_offset = it->line_count - it->visible_line_count;
            }
            else
                it->scroll_offset = 0;
            if (it->scroll_offset < 0)
                it->scroll_offset = 0;
            return;
        }
    }
}

// Scrolls a multi-line box one line further from the end of its text.
// The scroll offset counts lines back from the end, and typing returns it to 0.
void Edits::ScrollUp(int id)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id && it->multi_line)
        {
            it->scroll_offset++;
            if (it->line_count - it->visible_line_count > 0)
            {
                if (it->line_count - it->visible_line_count < it->scroll_offset)
                    it->scroll_offset = it->line_count - it->visible_line_count;
                return;
            }
            it->scroll_offset = 0;
            return;
        }
    }
}

void Edits::ScrollDown(int id)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id && it->multi_line)
        {
            if (it->scroll_offset > 0)
                it->scroll_offset--;
            return;
        }
    }
}

void Edits::SetFirstVisibleLine(int id, int line)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id && it->multi_line)
        {
            it->first_visible_line = line;
            return;
        }
    }
}

int Edits::GetFirstVisibleLine(int id)
{
    int first_line = 0;
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id && it->multi_line)
        {
            first_line = it->first_visible_line;
            break;
        }
    }
    return first_line;
}

// Sets how many lines show, and applies a scroll offset that was waiting for that count.
void Edits::SetVisibleLineCount(int id, int count)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id && it->multi_line)
        {
            it->visible_line_count = count;
            if (it->pending_scroll < 0)
                break;
            it->scroll_offset = it->pending_scroll;
            it->pending_scroll = -1;
            if (it->line_count - it->visible_line_count > 0)
            {
                if (it->line_count - it->visible_line_count < it->scroll_offset)
                    it->scroll_offset = it->line_count - it->visible_line_count;
            }
            else
                it->scroll_offset = 0;
            if (it->scroll_offset < 0)
                it->scroll_offset = 0;
            return;
        }
    }
}

int Edits::GetVisibleLineCount(int id)
{
    int count = 0;
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id && it->multi_line)
        {
            count = it->visible_line_count + 1;
            break;
        }
    }
    return count;
}

void Edits::SetLineCount(int id, int count)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id && it->multi_line)
        {
            it->line_count = count;
            return;
        }
    }
}

int Edits::GetLineCount(int id)
{
    int count = 0;
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id && it->multi_line)
        {
            count = it->line_count;
            break;
        }
    }
    return count;
}

void Edits::SetBorderVisible(int id, char visible)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            it->border_visible = visible;
            return;
        }
    }
}

void Edits::SetPasswordMode(int id, char password)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            it->password = password;
            return;
        }
    }
}

void Edits::SetShowStrength(int id, char show)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            it->show_strength = show;
            return;
        }
    }
}

void Edits::DisallowDoubleSpaces(int id)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            it->no_double_spaces = true;
            return;
        }
    }
}

void Edits::DisallowLeadingSpace(int id)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            it->no_leading_space = true;
            return;
        }
    }
}

void Edits::SetAllowSpaces(int id, char allow)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            it->allow_spaces = allow;
            return;
        }
    }
}

void Edits::SetCharFilter(int id, int char_filter)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            it->char_filter = char_filter;
            return;
        }
    }
}

void Edits::SetMaxLength(int id, int max_length)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            it->max_length = max_length;
            return;
        }
    }
}

void Edits::SetEditable(int id, char editable)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            it->editable = editable;
            return;
        }
    }
}

// Replaces a box's text and marks the text as changed.
void Edits::SetText(int id, String text)
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            if (it->multi_line)
                it->scroll_offset = 10000;
            it->text = text;
            text_changed = true;
            break;
        }
    }
}

// The text of the box with the id; a number box below its least value reads as that value.
String Edits::GetText(int id)
{
    String text = "";
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            if (it->numeric)
            {
                if (it->text.Length() < 1)
                    it->text = "";
                else if (StrToInt(it->text) < it->min_value)
                    it->text = IntToStr(it->min_value);
            }
            text = it->text;
            break;
        }
    }
    return text;
}

int Edits::GetTextLength(int id)
{
    int length = 0;
    vector<Edit>::iterator it;
    for (it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            length = it->text.Length();
            break;
        }
    }
    return length;
}

// Whether the text uses fewer than three different characters; account names and passwords like that are refused.
bool Edits::HasFewDistinctChars(String text)
{
    int i;
    bool few_distinct;
    char first_char;
    char second_char;
    int distinct_count = 0;
    few_distinct = true;
    if (text.Length() >= 1)
    {
        i = 1;
        while (i <= text.Length())
        {
            if (distinct_count == 2 && text[i] != first_char && text[i] != second_char)
                distinct_count++;
            if (distinct_count == 1 && text[i] != first_char)
            {
                second_char = text[i];
                distinct_count++;
            }
            if (!distinct_count)
            {
                first_char = text[i];
                distinct_count++;
            }
            i++;
        }
    }
    if (distinct_count >= 3)
        few_distinct = false;
    return few_distinct;
}

// A loose email test: five characters or more, an @ after the first, and a dot after the @.
bool Edits::IsValidEmail(String address)
{
    int i;
    char has_at = false;
    bool valid = false;
    if (address.Length() >= 5)
    {
        for (i = 2; i <= address.Length(); i++)
        {
            if (address[i] == '@')
                has_at = true;
            if (address[i] == '.' && has_at)
                valid = true;
        }
    }
    return valid;
}

// Hides and locks the chat box for a while, when the server says so.
void Edits::LockChatInput()
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (it->id == EditId_Chat)
        {
            it->editable = false;
            it->visible = false;
            break;
        }
    }
    chat_locked = true;
    lock_countdown = 150;
}
