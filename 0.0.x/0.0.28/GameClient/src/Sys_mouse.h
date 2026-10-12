#ifndef Sys_mouseH
#define Sys_mouseH
#define CHARACTER_CREATE_DIALOG 2
#define TRAIN_CONFIRM_TICKS 200

#define CHARACTER_SELECT_COOLDOWN_TICKS 10

enum ScreenMode
{
    ScreenMode_Intro = 1,
    ScreenMode_Menu = 2,
    ScreenMode_Game = 3
};
#define EDIT_LINE_HEIGHT 20
#define POST_SUBJECT_EDIT 20

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
    void HandleWheel(int direction);
    void HandleMouseUp();
};

void Sys_mouse_HandleClick(int *mouse, int x, int y, unsigned int button);

#endif
