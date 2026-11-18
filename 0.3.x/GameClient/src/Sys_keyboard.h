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
    unsigned char key_waits[69];
    char key_flags[4];
    unsigned char key_delay;
    unsigned char backspace_wait;
    unsigned char tab_wait;
    unsigned char enter_wait;
    unsigned char menu_enter_wait;
    unsigned char grid_enter_wait;
    char walk_key_held;
    bool wasd_key_used;
    bool attack_key_used;
    bool space_key_used;
    bool delete_pressed;
    unsigned char walk_direction;
    unsigned char last_walk_direction;

    KeyBoard();
    ~KeyBoard();
    char WasDeletePressed();
    void PollKeyboard();
    bool UpdateWalkDirection();
    bool HandleMenuKeys();
    bool HandleGameKeys();
    bool HandleEditKeys();
    bool HandleTypedKeys();
    bool TakeKeyDelay();
    bool TakeKeyRepeat(unsigned char key);
    void ResetKeyRepeat(unsigned char key);
    bool IsAnyWindowOpen();
    void ReleaseModifierKeys();
};

#endif
