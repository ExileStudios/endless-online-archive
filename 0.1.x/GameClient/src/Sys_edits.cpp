#include <vcl.h>
#pragma hdrstop

#include "Sys_edits.h"

#pragma package(smart_init)

Edits::Edits()
{
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

// Types into the focused box when it is editable and not full.
void Edits::TypeText(String text)
{
    vector<Edit>::iterator it = edits.begin();
    while (it != edits.end())
    {
        if (it->focused && it->editable && it->text.Length() < it->max_length)
        {
            String old_text = it->text;
            if (text.Length() >= 1)
            {
                it->dirty = true;
                if (it->multi_line)
                    it->scroll_offset = 0;
            }
            if (it->numeric)
            {
                if (text == "0" || text == "1" || text == "2" || text == "3" || text == "4" || text == "5" ||
                    text == "6" || text == "7" || text == "8" || text == "9")
                {
                    if (it->max_value < 10)
                    {
                        if (StrToInt(text) <= it->max_value)
                            it->text = text;
                        else
                            it->text = IntToStr(it->max_value);
                    }
                    else
                    {
                        String new_text = it->text + text;
                        if (new_text.Length() > 0)
                        {
                            if (StrToInt(new_text) <= it->max_value)
                            {
                                if (StrToInt(new_text) >= it->min_value)
                                    it->AppendText(text);
                                else
                                    it->text = IntToStr(it->min_value);
                            }
                            else
                                it->text = IntToStr(it->max_value);
                        }
                    }
                }
            }
            else
            {
                // Case modes: 1 upper case, 2 lower case, 3 a capital first letter.
                if (it->case_mode == EditCaseMode_Upper)
                    text = UpperCase(text);
                if (it->case_mode == EditCaseMode_Lower)
                    text = LowerCase(text);
                if (it->case_mode == EditCaseMode_CapitalFirst && it->text.Length() <= 0)
                    text = UpperCase(text);
                if (!it->char_filter)
                    it->AppendText(text);
                else
                {
                    char allowed = true;
                    String filter_text = text;
                    // Each filter level refuses more: punctuation, quotes and accented letters,
                    // then the marks . _ - ~ @, then digits, then spaces.
                    if (it->char_filter >= EditCharFilter_NoPunctuation)
                    {
                        if (filter_text == "'" || filter_text == "," || filter_text == "<" || filter_text == ">" ||
                            filter_text == ";" || filter_text == ":" || filter_text == "!" || filter_text == "=" ||
                            filter_text == "#" || filter_text == "$" || filter_text == "%" || filter_text == "&" ||
                            filter_text == "?" || filter_text == "\\" || filter_text == "+" || filter_text == "/" ||
                            filter_text == "^" || filter_text == "(" || filter_text == ")" || filter_text == "*" ||
                            filter_text == '~' || filter_text == '[' || filter_text == ']' || filter_text == '|' ||
                            filter_text == (char)-27 || filter_text == (char)-10 || filter_text == (char)-28 ||
                            filter_text == (char)-59 || filter_text == (char)-42 || filter_text == (char)-60 ||
                            text == '"' || filter_text == (char)-89 || filter_text == (char)-24 ||
                            filter_text == (char)-25 || filter_text == (char)-32 || filter_text == (char)-23)
                            allowed = false;
                    }
                    if (it->char_filter >= EditCharFilter_AlphanumericSpaces)
                    {
                        if (filter_text == "." || filter_text == "_" || filter_text == "-" || filter_text == "~" ||
                            filter_text == "@")
                            allowed = false;
                    }
                    if (it->char_filter >= EditCharFilter_LettersSpaces)
                    {
                        if (filter_text == "0" || filter_text == "1" || filter_text == "2" || filter_text == "3" ||
                            filter_text == "4" || filter_text == "5" || filter_text == "6" || filter_text == "7" ||
                            filter_text == "8" || filter_text == "9")
                            allowed = false;
                    }
                    if (it->char_filter >= EditCharFilter_Letters)
                    {
                        if (filter_text == " ")
                            allowed = false;
                    }
                    if (allowed)
                        it->AppendText(text);
                }
            }
            if (it->change_notify && old_text != it->text)
                text_changed = true;
            break;
        }
        it++;
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

// Adds a box, which takes the focus from the others.
void Edits::AddEdit(int id, int screen_group, int x, int y, int width, int height)
{
    UnfocusAll();
    Edit handle(id, screen_group, x, y, width, height);
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

String Edits::GetText(int id)
{
    String text = "";
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
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
