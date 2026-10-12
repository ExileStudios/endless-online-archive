#ifndef Sys_editsH
#define Sys_editsH

#include <vector.h>

enum EditId
{
    EditId_Chat = 11,
    EditId_FormFirstRow = 14,
    EditId_FormSecondRow = 15
};

#include "Sys_edit.h"

// The text boxes on screen, which one has the focus, the caret blink, and the lock on the chat box.
struct Edits
{
    bool caret_blink_on;
    bool caret_enabled;
    bool chat_locked;
    char pad_0x3[5];
    vector<Edit> edits;
    int current_index;
    bool tick_redraw;
    bool text_changed;
    int lock_countdown;
    char pad_0x34[4];

    Edits();
    ~Edits();
    void Clear();
    void TimerTick();
    bool ConsumeTickRedraw();
    void TypeText(String text);
    void TypeLineBreak();
    void ClearFocusedText();
    void Backspace();
    void AddEdit(int id, unsigned int screen_group, int x, int y, int width, int height);
    void RemoveEdit(int id);
    void UnfocusAll();
    void FocusClickedEdit(Edit edit_x);
    void FocusEdit(int id);
    bool HasDirtyEdit();
    char ConsumeTextChanged();
    bool IsFocused(int id);
    bool ConsumeDirty(int id);
    void FocusNext();
    Edit GetCurrentEdit();
    unsigned int GetCount();
    void MoveNext();
    void MoveFirst();
    bool IsAtEnd();
    void SetNumericRange(int id, int min_value, int max_value);
    void SetCaseMode(int id, int case_mode);
    void EnableChangeNotify(int id);
    void SetVisible(int id, char visible);
    void SetTextColor(int id, int color);
    void SetPosition(int id, int x, int y);
    void SetBackgroundVisible(int id, char visible);
    void SetMultiLine(int id, char multi_line);
    int GetScrollOffset(int id);
    void SetPendingScroll(int id, int offset);
    void SetScrollOffset(int id, int offset);
    void ScrollUp(int id);
    void ScrollDown(int id);
    void SetFirstVisibleLine(int id, int line);
    int GetFirstVisibleLine(int id);
    void SetVisibleLineCount(int id, int count);
    int GetVisibleLineCount(int id);
    void SetLineCount(int id, int count);
    int GetLineCount(int id);
    void SetBorderVisible(int id, char visible);
    void SetPasswordMode(int id, char password);
    void SetCharFilter(int id, int char_filter);
    void SetMaxLength(int id, int max_length);
    void SetEditable(int id, char editable);
    void SetText(int id, String text);
    String GetText(int id);
    int GetTextLength(int id);
    bool HasFewDistinctChars(String text);
    bool IsValidEmail(String address);
    void LockChatInput();
};

#endif
