#ifndef Sys_editsH
#define Sys_editsH

enum EditCharFilter
{
    EditCharFilter_Printable = 1,
    EditCharFilter_Email = 2,
    EditCharFilter_Password = 3,
    EditCharFilter_Alphanumeric = 4,
    EditCharFilter_Letters = 5
};

enum EditGroup
{
    EditGroup_Menu = 2,
    EditGroup_Game = 3,
    EditGroup_InputForm = 105,
    EditGroup_MsgBoard = 109,
    EditGroup_All = 240
};

enum EditCaseMode
{
    EditCaseMode_None = 0,
    EditCaseMode_Upper = 1,
    EditCaseMode_Lower = 2,
    EditCaseMode_CapitalFirst = 3,
    EditCaseMode_Code = 4,
    EditCaseMode_Name = 5
};

#include <new>
#include <vector.h>

#include "Sys_edit.h"

enum EditId
{
    EditId_AccountName = 1,
    EditId_Password = 2,
    EditId_AccountPassword = 3,
    EditId_ConfirmPassword = 4,
    EditId_OldPassword = 5,
    EditId_NewPassword = 6,
    EditId_ConfirmNewPassword = 7,
    EditId_AccountDetailA = 8,
    EditId_AccountDetailB = 9,
    EditId_Email = 10,
    EditId_Chat = 11,
    EditId_InputBox = 12,
    EditId_FormFirstRow = 14,
    EditId_FormSecondRow = 15,
    EditId_Amount = 13,
    EditId_FormThirdRow = 16,
    EditId_FormFourthRow = 17,
    EditId_FormFifthRow = 18,
    EditId_Code = 22,
    EditId_Captcha = 23
};

// The text boxes on screen, which one has the focus, the caret blink, and the lock on the chat box.
struct Edits
{
    bool caret_blink_on;
    bool caret_enabled;
    bool chat_locked;
    int default_text_color;
    vector<Edit> edits;
    int current_index;
    bool tick_redraw;
    bool text_changed;
    int lock_countdown;

    Edits(int default_color);
    ~Edits();
    void Clear();
    void TimerTick();
    bool ConsumeTickRedraw();
    void TypeText(String text);
    void TypeChar(char key);
    void TypeLineBreak();
    char ClearFocusedText();
    void Backspace();
    int GetPasswordStrength(String password);
    int GetPasswordProblem(int id);
    void AddEdit(int id, int screen_group, int x, int y, int width, int height);
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
    void SetShowStrength(int id, char show);
    void DisallowDoubleSpaces(int id);
    void DisallowLeadingSpace(int id);
    void SetAllowSpaces(int id, char allow);
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
