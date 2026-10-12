#ifndef Sys_keyboardH
#define Sys_keyboardH

enum KeyboardLayout
{
    KeyboardLayout_Dutch = 1,
    KeyboardLayout_Swedish = 2,
    KeyboardLayout_French = 3
};

// Keyboard input: the menu and game keys, walking with the arrows, and typing into the edit boxes.
struct KeyBoard
{
    String last_key;
    String repeat_key;
    int repeat_delay;
    int repeat_interval;
    int backspace_delay;
    int tab_delay;
    int key_delay;
    bool arrow_held;
    bool ctrl_attacked;
    bool two_arrows_held;
    String walk_direction;
    String first_direction;

    KeyBoard();
    ~KeyBoard();
    bool IsArrowKeyDown();
    String GetWalkDirection();
    void PollKeyboard();
};

#endif
