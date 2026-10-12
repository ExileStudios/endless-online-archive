#include <vcl.h>
#pragma hdrstop

#include "Sys_edits.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
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
    int __slot1;
    int __slot2;
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
                it->visible = 1;
                it->editable = 1;
            }
            if (it->change_notify && caret_enabled)
                text_changed = true;
            it->dirty = 1;
            break;
        }
    }
    if (caret_blink_on)
        caret_blink_on = false;
    else
        caret_blink_on = true;
}

bool Edits::ConsumeTickRedraw()
{
    bool redraw = 0;
    if (tick_redraw)
    {
        tick_redraw = false;
        redraw = 1;
    }
    return redraw;
}

// Types into the focused box when it is editable and not full.
void Edits::TypeText(String text)
{
    char allowed;
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (!it->focused)
            continue;
        if (!it->editable)
            continue;
        if (it->text.Length() < it->max_length)
        {
            String old_text = it->text;
            if (text.Length() >= 1)
            {
                it->dirty = 1;
                if (it->multi_line)
                    it->scroll_offset = 0;
            }
            // A numeric box keeps its value inside its range; with a top value under 10 a digit replaces it.
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
                                {
                                    it->AppendText(text);
                                }
                                else
                                {
                                    it->text = IntToStr(it->min_value);
                                }
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
                if (it->case_mode == 1)
                    text = UpperCase(text);
                if (it->case_mode == 2)
                    text = LowerCase(text);
                if (it->case_mode == 3)
                {
                    if (it->text.Length() <= 0)
                        text = UpperCase(text);
                }
                if (!it->char_filter)
                    it->AppendText(text);
                else
                {
                    allowed = 1;
                    char saved_v90 = allowed;
                    String filter_text = text;
                    // Each filter level refuses more: punctuation, quotes and accented letters,
                    // then the marks . _ - ~ @, then digits, then spaces.
                    if (it->char_filter >= 1)
                    {
                        if (filter_text == "'" || filter_text == "," || filter_text == "<" || filter_text == ">" ||
                            filter_text == ";" || filter_text == ":" || filter_text == "!" || filter_text == "=" ||
                            filter_text == "#" || filter_text == "$" || filter_text == "%" || filter_text == "&" ||
                            filter_text == "?" || filter_text == "\\" || filter_text == "+" || filter_text == "/" ||
                            filter_text == "^" || filter_text == "(" || filter_text == ")" || filter_text == "*" ||
                            filter_text == (char)126 || filter_text == (char)91 || filter_text == (char)93 ||
                            filter_text == (char)124 || filter_text == (char)(-27) || filter_text == (char)(-10) ||
                            filter_text == (char)(-28) || filter_text == (char)(-59) || filter_text == (char)(-42) ||
                            filter_text == (char)(-60) || text == (char)34 || filter_text == (char)(-89) ||
                            filter_text == (char)(-24) || filter_text == (char)(-25) || filter_text == (char)(-32) ||
                            filter_text == (char)(-23))
                            allowed = 0;
                    }
                    if (it->char_filter >= 2)
                    {
                        if (filter_text == "." || filter_text == "_" || filter_text == "-" || filter_text == "~" ||
                            filter_text == "@")
                            allowed = 0;
                    }
                    if (it->char_filter >= 3)
                    {
                        if (filter_text == "0" || filter_text == "1" || filter_text == "2" || filter_text == "3" ||
                            filter_text == "4" || filter_text == "5" || filter_text == "6" || filter_text == "7" ||
                            filter_text == "8" || filter_text == "9")
                            allowed = 0;
                    }
                    if (it->char_filter >= 4)
                    {
                        if (filter_text == " ")
                            allowed = 0;
                    }
                    if (allowed)
                        it->AppendText(text);
                }
            }
            if (it->change_notify && old_text != it->text)
                text_changed = true;
            break;
        }
    }
}

// Enter in the focused box: a multi-line box gets a line break.
void Edits::TypeLineBreak()
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (!it->focused)
            continue;
        if (!it->editable)
            continue;
        if (it->text.Length() < it->max_length)
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

// Empties the focused box; a numeric box goes back to its lowest value.
void Edits::ClearFocusedText()
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (it->focused && it->editable)
        {
            if (it->text.Length() >= 1)
                it->dirty = 1;
            if (it->multi_line)
                it->scroll_offset = 0;
            if (it->change_notify)
                text_changed = true;
            if (it->numeric)
                it->text = IntToStr(it->min_value);
            else
                it->ClearText();
            break;
        }
    }
}

void Edits::Backspace()
{
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (it->focused && it->editable)
        {
            if (it->text.Length() >= 1)
                it->dirty = 1;
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
void Edits::AddEdit(int id, unsigned int screen_group, int x, int y, int width, int height)
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
        it->focused = 0;
}

// Moves the focus to the box that was clicked.
void Edits::FocusClickedEdit(Edit edit_x)
{
    UnfocusAll();
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (it->id == String(edit_x.id))
        {
            it->focused = 1;
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
            it->focused = 1;
            return;
        }
    }
}

bool Edits::HasDirtyEdit()
{
    bool dirty = 0;
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (it->dirty)
        {
            dirty = 1;
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
    bool focused = 0;
    vector<Edit>::iterator it;
    for (it = edits.begin(); it != edits.end(); it++)
    {
        if (id != it->id)
            continue;
        if (it->focused)
        {
            focused = 1;
            break;
        }
    }
    return focused;
}

bool Edits::ConsumeDirty(int id)
{
    bool dirty = 0;
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id && it->dirty)
        {
            it->dirty = 0;
            dirty = 1;
            break;
        }
    }
    return dirty;
}

// Moves the focus to the next box, round to the first after the last.
void Edits::FocusNext()
{
    char focus_next = 0;
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (focus_next)
        {
            it->focused = 1;
            focus_next = 0;
            break;
        }
        if (it->focused)
        {
            it->focused = 0;
            focus_next = 1;
        }
    }
    if (focus_next)
        edits[0].focused = 1;
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
    if (GetCount() > (unsigned int)current_index)
        current_index++;
}

// With MoveNext, IsAtEnd and GetCurrentEdit, walks the boxes in order.
void Edits::MoveFirst()
{
    if (GetCount() >= 1)
        current_index = 0;
    else
        current_index = -1;
}

bool Edits::IsAtEnd()
{
    bool at_end = 0;
    if (GetCount() == (unsigned int)current_index || !GetCount())
        at_end = 1;
    return at_end;
}

// Makes the box numeric within the range, with the top value capped.
void Edits::SetNumericRange(int id, int min_value, int max_value)
{
    int limit;
    vector<Edit>::iterator it;
    for (it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id)
        {
            it->numeric = true;
            it->min_value = min_value;
            limit = max_value;
            it->max_value = limit;
            if (limit > 10000000)
                it->max_value = 10000000;
            return;
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
            return;
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
    int max_offset;
    int wanted;
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id && it->multi_line)
        {
            wanted = offset;
            it->scroll_offset = wanted;
            max_offset = it->line_count - it->visible_line_count;
            if (max_offset > 0)
            {
                if (wanted > max_offset)
                    it->scroll_offset = max_offset;
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
    int max_offset;
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id && it->multi_line)
        {
            it->scroll_offset++;
            max_offset = it->line_count - it->visible_line_count;
            if (max_offset > 0)
            {
                if (max_offset < it->scroll_offset)
                    it->scroll_offset = max_offset;
            }
            else
                it->scroll_offset = 0;
            break;
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
    int max_offset;
    for (vector<Edit>::iterator it = edits.begin(); it != edits.end(); it++)
    {
        if (id == it->id && it->multi_line)
        {
            it->visible_line_count = count;
            if (it->pending_scroll >= 0)
            {
                it->scroll_offset = it->pending_scroll;
                it->pending_scroll = -1;
                max_offset = it->line_count - it->visible_line_count;
                if (max_offset > 0)
                {
                    if (max_offset < it->scroll_offset)
                        it->scroll_offset = max_offset;
                }
                else
                    it->scroll_offset = 0;
                if (it->scroll_offset < 0)
                    it->scroll_offset = 0;
            }
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
    bool few_distinct;
    char first_char;
    char second_char;
    int i = 0;
    int distinct_count = 0;
    few_distinct = 1;
    if (text.Length() >= 1)
    {
        for (i = 1; i <= text.Length(); i++)
        {
            if (distinct_count == 2)
            {
                if (text[i] != first_char)
                {
                    if (text[i] != second_char)
                        distinct_count++;
                }
            }
            if (distinct_count == 1)
            {
                if (text[i] != first_char)
                {
                    second_char = text[i];
                    distinct_count++;
                }
            }
            if (!distinct_count)
            {
                first_char = text[i];
                distinct_count++;
            }
        }
    }
    if (distinct_count >= 3)
        few_distinct = 0;
    i = 0;
    return few_distinct;
}

// A loose email test: five characters or more, an @ after the first, and a dot after the @.
bool Edits::IsValidEmail(String address)
{
    bool valid;
    char has_at = 0;
    valid = 0;
    if (address.Length() >= 5)
    {
        for (int i = 2; i <= address.Length(); i++)
        {
            if (address[i] == '@')
                has_at = 1;
            if (address[i] == '.' && has_at)
                valid = 1;
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
            it->editable = 0;
            it->visible = 0;
            break;
        }
    }
    chat_locked = true;
    lock_countdown = 150;
}
