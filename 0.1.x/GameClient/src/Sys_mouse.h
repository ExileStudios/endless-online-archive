#ifndef Sys_mouseH
#define Sys_mouseH

enum WheelDirection
{
    WheelDirection_Up = 1,
    WheelDirection_Down = 2
};

enum MouseButton
{
    MouseButton_Left = 0,
    MouseButton_Right = 1
};

#define CHANGE_PASSWORD_DIALOG 1
#define CHARACTER_CREATE_DIALOG 2
#define CHARACTER_SELECT_COOLDOWN_TICKS 10
#define EDIT_LINE_HEIGHT 20
#define POST_MESSAGE_EDIT 21
#define POST_SUBJECT_EDIT 20
#define TRAIN_CONFIRM_TICKS 200

enum ScreenMode
{
    ScreenMode_Intro = 1,
    ScreenMode_Menu = 2,
    ScreenMode_Game = 3
};

// The mouse: its last position in game coordinates, and the move, button, wheel and release handlers.
struct Mouser
{
    int x;
    int y;
    char moved;

    Mouser();
    ~Mouser();
    void SetPosition(int cursor_x, int cursor_y);
    void HandleMove();
    void HandleMouseDown(int click_x, int click_y, int button);
    void HandleWheel(int direction);
    void HandleMouseUp();
    void OpenChatPanel(bool play_sound);
    void OpenPlayerlistPanel(bool play_sound);
    void OpenStatusPanel(bool play_sound);
    void OpenInventoryPanel(bool play_sound);
    void ToggleMinimap(bool play_sound);
    void OpenGroupPanel(bool play_sound);
    void OpenSpellPanel(bool play_sound);
    void OpenSkillPanel(bool play_sound);
    void OpenSettingsPanel(bool play_sound);
    void OpenHelpPanel(bool play_sound);
    void OpenLoginScreen();
};

#endif
